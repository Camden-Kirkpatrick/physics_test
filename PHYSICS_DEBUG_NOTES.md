# Physics Debugging Notes — Bouncing Boxes

A record of the bugs in the box-bounce simulation, what caused them, and what
fixed them. Written so I can come back later and remember *why* the code looks
the way it does. Read top to bottom — each problem builds on the last.

The relevant code lives in `src/gameLayer/gameMain.cpp` (`updateGame`) and
`src/gameLayer/physics.hpp` (`PhysicalEntity::updateForces`).

---

## The setup

- A window with some boxes falling under gravity and bouncing off the 4 window
  edges.
- Each box has: `transform.pos` (top-left corner), `velocity`, `acceleration`,
  and `drag`.
- Screen coordinates: **+y is DOWN**. So "up" is negative velocity. This sign
  convention trips you up constantly — keep it in mind.
- Each frame: apply gravity, integrate (move by velocity), resolve wall
  collisions, draw.

Two behaviors I wanted, and the rule that governs them:
- **drag == 0** → no energy is ever lost → boxes bounce forever at the same height.
- **drag > 0** → boxes lose energy over time → they eventually settle and rest.
  (This is how real life works: drag is the only thing that removes energy.)

---

## Problem 1 — Boxes stopped, then randomly started moving again (with drag == 0)

**Symptom:** with drag at 0, boxes should bounce forever. Instead they slowly
lost height, settled to the floor, and then twitched back to life at random.

**Root cause A — clamping the bounce deleted energy.**
The original bounce snapped the box exactly to the edge:

```cpp
if (pos.y >= bottom && velocity.y > 0) { pos.y = bottom; velocity.y *= -1; }
```

When a box overshoots the floor by some distance in a frame, `pos.y = bottom`
throws that overshoot away. In a real bounce the box would travel that distance
back *up*. Deleting it bleeds a little energy every single bounce — so boxes
stopped even though drag was 0.

**Root cause B — bounce ran before the integration step.**
The velocity flip happened before gravity was applied for the frame, which made
the effective restitution depend on frame timing.

**Fix:** reflect the overshoot instead of clamping, and integrate first.

```cpp
box.updateForces(deltaTime);                       // integrate FIRST
if (pos.y >= bottom && velocity.y > 0) {
    pos.y = 2.0f * bottom - pos.y;                 // reflect (mirror across edge)
    velocity.y *= -1.0f;
}
```

**Why `2*edge - pos`:** treat the wall as a mirror. If the box landed 30px past
the floor, `2*bottom - pos` puts it 30px back *inside*, instead of deleting the
30px. Energy preserved.
For the top/left walls the edge is 0, so `2*0 - pos` simplifies to `-pos`.

**Why the "random restart":** once a box had decayed to the floor, tiny
frame-time variations were randomly re-injecting energy. That was a preview of
Problem 3.

---

## Problem 2 — With drag on, boxes barely bounced (and I misread it as "falling faster")

**Symptom:** turned drag on and the boxes seemed to drop and not come back up.

**What was actually going on:** the drag is *quadratic* (`v * |v|`). With
`drag = 0.5` and `g = 50000`, terminal velocity is
`sqrt(g/drag) = sqrt(100000) ≈ 316 px/s`. So a box could never hit the floor
faster than ~316 px/s, and on the way up gravity + drag together killed it in
about half a pixel. The bounce was real but invisible.

**Fix:** the drag coefficient was just way too big. Pick it from the terminal
velocity you want to see: `drag = g / v_terminal²`. Values in the
`0.00001 – 0.001` range give visible, decaying bounces.

**Conceptual clarification (this confused me):** drag opposes *motion*, not
gravity. When falling, drag points up; when rising, drag points down. It's a
brake on velocity, never a lift. **Drag can never make something fall faster
than free-fall.** What looked like "falling faster" was really the boxes losing
bounce height and settling at the bottom, vs. bouncing to the ceiling forever
when drag was 0.

---

## Problem 3 — Boxes jittered forever at the bottom instead of resting

**Symptom:** with drag on, a box would decay down to the floor and then vibrate
in place forever instead of stopping.

**Root cause — the sim has no "normal force."**
In real life, a box resting on the floor gets an upward normal force from the
floor that exactly cancels gravity, so it sits still. This sim has nothing like
that. The *only* response to hitting the floor is a bounce. So every frame:
1. gravity adds a little downward speed (`g * dt`),
2. the box dips just below the floor,
3. the bounce flips that speed back upward.

Gravity nudges, bounce returns, forever. That loop **is** the jitter. Drag
can't save it either — at ~20 px/s, quadratic drag removes almost nothing, while
gravity re-adds the full `g*dt` every frame.

**Fix — a "rest pin" that fakes the normal force.** When a floor bounce comes
back too slow to be visible, pin the box to the floor and zero its velocity
instead of bouncing it:

```cpp
if (pos.y >= bottom && velocity.y > 0) {
    pos.y = 2.0f * bottom - pos.y;
    velocity.y *= -1.0f;
    if (-velocity.y < restThreshold) {   // upward speed below cutoff?
        pos.y = bottom;                  //   -> pin to floor
        velocity.y = 0.0f;               //   -> full stop
    }
}
```

Note `-velocity.y`: after the flip the box moves up, and up is negative, so its
*speed* is `-velocity.y`.

**Important:** the pin only matters when drag > 0 and boxes are meant to settle.
With drag == 0 the bounces never get small enough to trip it, so it never fires
and boxes bounce forever — exactly as intended.

**Also required for the pin to stick:** integrate *first*, then collide. If the
collision runs before integration, `updateForces` immediately undoes the pin and
the box gets drawn slightly above the floor at `bottom + g*dt²` — which wobbles
by a fraction of a pixel with frame time and flickers by 1px. Collide last.

---

## Problem 4 — Deriving the rest threshold

`restThreshold` is the cutoff speed below which a bounce is "too small to see."

A bounce leaving the floor at speed `u` rises to height `h = u² / (2g)`
(kinematics: rises until gravity eats all the speed). Solve for the speed that
rises exactly `restRise` pixels:

```
u = sqrt(2 * g * restRise)
```

So `restThreshold = sqrt(2 * g * restRise)`. With `g = 5000`, `restRise = 1`:
`sqrt(2*5000*1) = 100 px/s`. Any bounce slower than 100 px/s rises < 1px.

**The `2` inside the sqrt is real physics** (from `h = u²/(2g)`), NOT a fudge
factor. Removing it silently changes `restRise` to mean half a pixel.

---

## Problem 5 — The jitter came back "out of nowhere" (the sneaky one)

**Symptom:** everything worked, then intermittently — seemingly at random — the
boxes jittered again. Then it was fine. Then it happened again.

**The clue was the word "intermittent."** A constant bug jitters constantly. A
bug that fires *sometimes* is tied to something that varies — and the thing that
varies frame to frame is `deltaTime`.

**Root cause — a fixed threshold vs. a frame-dependent nudge.**
A resting box's per-frame gravity nudge is `g * deltaTime`. The pin only fires
when that nudge is *below* `restThreshold`. If `restThreshold` is a fixed number
(`100`), the pin works only while `g * deltaTime < 100`, i.e. `deltaTime < 0.02s`
(~50 fps). On any slow/hitched frame `deltaTime` spikes (and the code even clamps
it up to `0.05`), so `g * deltaTime` can jump to `250`, blow past `100`, the pin
doesn't fire, and the box hops. Random frame hitches → random jitter.

**Fix — floor the threshold so it scales WITH the nudge:**

```cpp
float restThreshold = fmaxf(sqrtf(2.0f * g * restRise), 2.0f * g * deltaTime);
```

The `2 * g * deltaTime` term grows on slow frames right alongside the nudge. The
residual `g*dt` can never exceed `2*g*dt`, so the pin fires no matter how slow
the frame runs. Frame-time independence restored.

**Hard-won lesson:** I (the AI) had earlier called this second term "redundant,
safe to delete," because on *fast* frames `100 > 2*g*dt`. That was wrong — it's
only redundant on fast frames. On slow frames it's the whole point. Deleting it
is what brought the jitter back. **Don't remove it.**

Trade-off accepted: on a genuinely hitched frame the threshold gets large, so a
real small bounce might get pinned a touch early. A box quietly stopping during a
hitch is far less noticeable than jitter, so this is fine.

---

## Final state of the key code

```cpp
float g = 5000.0f;
box.applyGravity(g);

// integrate FIRST, collide after (so the rest-pin isn't undone before drawing)
box.updateForces(deltaTime);

// speed below which a bounce rises < restRise pixels (invisible) -> treat as rest.
// second term floors it so the per-frame g*dt residual is always caught, even on
// slow/hitched frames. DO NOT DELETE the second term.
float restRise = 1.0f; // px
float restThreshold = fmaxf(sqrtf(2.0f * g * restRise), 2.0f * g * deltaTime);

// reflect across the edge (energy-preserving) + flip; edge 0 => 2*edge-pos == -pos
if (box.transform.pos.y >= bottom && box.velocity.y > 0) {
    box.transform.pos.y = 2.0f * bottom - box.transform.pos.y;
    box.velocity.y *= -1.0f;
    if (-box.velocity.y < restThreshold) {   // too slow to be a visible bounce
        box.transform.pos.y = bottom;        //   -> pin to floor
        box.velocity.y = 0.0f;               //   -> stop
    }
}
// ...same reflect for top/left/right walls (no rest-pin needed there)...
```

---

## The mental models worth keeping

1. **Reflect, don't clamp.** Clamping a position to a wall silently deletes
   energy. `pos = 2*edge - pos` bounces the overshoot back and conserves it.
2. **Integrate first, collide last.** Whatever touches the box last is what gets
   drawn. Fix positions after moving, not before.
3. **A bounce-only sim can't rest on its own.** With no normal force, gravity
   re-nudges a grounded box forever. You need an explicit "close enough to
   stopped → pin it" rule. That rule fakes the normal force.
4. **Drag opposes motion, not gravity.** It can only ever slow a fall, never
   speed it up. Only drag removes energy; a perfect bounce doesn't.
5. **Intermittent bugs point at something that varies.** Here it was
   `deltaTime`. Any threshold compared against a per-frame quantity must scale
   with `deltaTime`, or it breaks on slow frames.
6. **"Redundant on the easy case" is not "redundant."** The frame-time floor
   looked useless at high fps and was essential at low fps. Test the hard case.
```
