# PLRU l1 cache amplification

This poc shows that l1 cache hits vs misses can be reliably timed even with
coarse timers. I.e. the timing difference can be arbitrarily increased by
abusing the Pseudo-LRU eviction strategy of the L1 cache.

This code in particular targets Tree PLRU strategies as found on most (?) modern CPUs.

## How does this work?

The Tree PLRU strategy is based on a tree structure that keeps track at every
node which side has been accessed recently. When an element needs to be
evicted, it just needs to follow the path of the pointers to the less recently
accessed side of the tree.
Some observations:
* if you need to evict multiple elements in a row, the strategy will alternate
  between the left and the right side of the tree.
* when filling the cache, we know which elements are neighbors in the tree:
  * 0 and 4, 1 and 5, ...
* we can keep an element alive by accessing its neighbor:
  * if element X is about to be evicted, access the neighbor
  * all "least recently used" markers at the nodes will be updated to point
    away from this node
  * repeat every n rounds

The code now works like this:
1) fill the cache set with $cacheSize (8) known elements
2) trigger code that might access this cache set
3) iterate through the 8 elements repeatedly. Every 4th access touches the
   neighbor of the element potentially replaced in step 2) to keep it alive

If the cache set was not used in step 2 then we only access the 8 elements
that are already in the cache. Nothing gets evicted and every access will be
fast (4 cycles on my CPU).
OTOH, if step 2 evicted an element from the cache, then we end up with a cache
eviction for every access except to the keep alive. I.e. we hit the l2 cache 3
out of 4 times (12 cycles instead of 4).

## Examples

```sh
$ grep name -m1 /proc/cpuinfo
model name      : Intel(R) Xeon(R) Gold 6154 CPU @ 3.00GHz
$ ./plru
[*] randomElements
      min: 55900
      max: 55912
      median: 55910
[*] no eviction
      min: 55902
      max: 55910
      median: 55910
[*] eviction
      min: 118748
      max: 118752
      median: 118750
[*] median diff: 62840 (112%)
```
