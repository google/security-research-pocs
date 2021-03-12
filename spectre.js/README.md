# Spectre JavaScript PoCs

This repo contains the code for https://leaky.page, a demo of Spectre in
JavaScript. Please check the website first since it already has a walkthrough on
how the demo works. In this writeup, we will go into a little bit more detail
and discuss design decisions.

## Cache Timing

As you've seen, the demo is timing the difference between L1 and L2 cache hits
instead of comparing L3 vs memory accesses. For details how the timer works,
please read https://leaky.page/plru.html.

While experimenting, the first version of the poc was using an L3 timer. This
has a few advantages:
* You can test multiple cache lines in the probe array after triggering the
  gadget once. This allows using a gadget that leaks a whole byte at once.
* You can trigger the gadget multiple times in a row before testing the probe
  array to increase the chance that it worked. With the L1 timer, you need to
  time every gadget run instead.
* The L1 timer is more finicky and requires small gadget code that doesn't
  perform too many unrelated memory accesses. We couldn't get some gadgets to
  work with the L1 timer even though they worked with the L3 timer without
  issues.

On the other hand, the advantages of the L1 timer are:
* You don't need a high precision timer. The L1 timer allows you to increase the
  timing difference between a hit and a miss by adjusting the number of loops.
  Especially since SharedArrayBuffers are now only available for "cross-origin
  isolated" sites, it can become more difficult to find a good timer.
* With the L3 timer, you need to evict the elements from the probe array before
  every run. To do so, you need to generate eviction sets which increases the
  setup time and can become a source of failure.
* The L1 timer made the memory layout inference faster and more reliable since
  fewer cache sets have to be checked.

For L3 cache eviction code, check out Pepe Vila's [evsets project](https://github.com/cgvwzq/evsets/tree/master/browser).

## Memory Layout Inference

The gadget needs to speculate over the array length check and assume that it
passes. This happens when the length is not in the L1 cache and the CPU has to
wait for the value. Though since we want the backing store pointer to be cached,
we need to use an object that is aligned so that the length field and the backing
store pointer are on different cache lines.
Both the L1 and L3 timers allow us to infer the memory layout, how this works is
described on https://leaky.page/memory.html.

## Cache Eviction

The most reliable method to get the gadget to be successful is by evicting the
length field from L3 cache, which for inclusive caches, will also evict it from
the lower levels.
However, this requires you to generate a minimized eviction set for the cache
line. If you use the L3 timer, you already have this eviction set, however for
the L1 timer demo, we tried to avoid this since it would introduce another
source for errors and make the setup time longer.
Instead, we tried to minimize the generated code of the Spectre gadget and
access random memory addresses at the same page offset. We noticed, that on some
CPUs the gadget works even if the length field is only evicted from the L1
cache, while on others you need to evict it from L2 or even L3. We made this a
configurable parameter in the demo and in practice, between 100 and 500 memory
accesses seem to be enough to make the gadget work (to evict from L3, you would
need to access >1000 random addresses depending on the cache size).

## Spectre Gadget Variants

The gadget implemented in the demo is a variant 1 gadget that's reading out of
bounds of a TypedArray. This was mainly chosen since it's very easy to
implement. This gadget can be mitigated by the compiler, however variant 4 in
particular seems to be infeasible to mitigate in software.

In our tests, the TypedArray gadget was only able to leak memory up to a
certain limit. This is likely limited by the internal representation of the
array index as a small integer (SMI) which allows reading up to 2^31 bytes.
During experimentation we also found other gadgets that were able to leak
using 64 bit offsets. However, making them work with the L1 timer proved to be
difficult.

## Does Speed Matter

A potential mitigation to Spectre exploits is to slow them down significantly to
make it unlikely that an attacker can leak valuable information during the
victim's page visit.
One thing to keep in mind is that some valuable secrets are very short, for
example XSRF tokens. If the attacker is able to leak arbitrary addresses (using
a 64 bit gadget), they can use the memory layout of the browser process and
follow pointers from the main binary to the secret they're interested in.
Assuming the attacker has to follow 10 pointers, that would require them to leak
only 80 bytes plus the length of the secret.

## Development

When writing an exploit for a CPU vulnerability in JavaScript there are many
parts that need to work and can interfere with each other.
Our approach to minimize this was as follows:
* We started with a poc written in C.
* Next: build a custom version of d8 (a commandline js interpreter) and expose
  all required primitives to JavaScript:
  * a high precision timer (rdtsc)
  * clflush
  * L3 cache eviction code
  * the spectre gadget
  * a memory leak to infer the page offset of JavaScript objects
  * a cache set oracle for a given address
* Write a scaffold in JavaScript that puts all parts together into a working
  exploit.
* Finally, replace the native parts one by one with JavaScript versions.

This allowed us to focus on individual parts and notice what is not working,
for example if the cache eviction is broken or the timer is too imprecise.
