/*

   binsearch_var.c -- Tighter variant on Binary Search.

   Les Kitchen <bs@po.ljk.id.au>, 26 March 2016.

   NOTE: The important code is in the function binsea() towards
   the end of this file.

   As recommended by the Free Software Foundation, 
      http://www.gnu.org/licenses/license-recommendations.html
   this small program is distributed under the liberal
   free-software Apache Licence.  In general, the licensing
   terms come into play only if you re-publish this code; they
   don't affect private, personal, unpublished use.  You may
   also have rights not affected by this licence, such as
   fair-dealing.  My intent is not to restrict your use
   unnecessarily, but to make clear the conditions under which
   you can make use of this code.

   Copyright 2016, Les Kitchen.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

/*

   Written in plain C rather than pseudo-code, so it can be
   compiled and run.  However, I've tried to avoid natural C
   constructs, like the ternary conditional operator, (?:),
   which might not make sense to those not familiar with C.

*/


/** This chunk compromises necessary top-level declarations and driver program. **/
#include <stdio.h>
#include <stdlib.h>
#define NOT_FOUND (-1)

int binsea( int key, int a[], int lo, int hi );
int b[] = { 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60 };
/* This is  C-ism to compute number of elements of the array */
int m = sizeof(b) / sizeof(b[0]);

int main ( char **argv, int argc ) {

  printf("Number of elements is %d\n", m);
  printf("Test 1: %d\n", binsea( 30, b, 0, m ));
  printf("Test 2: %d\n", binsea( 12, b, 0, m ));
  exit(0);

}

/*

   The code below is bereft of comments, so you can appreciate
   the bare algorithm uncluttered.  But you should read the
   block of comments below the code, in particular for the
   conventions used for specifying ranges.

*/

int binsea( int key, int a[], int lo, int hi ) {

  if ( hi-lo == 1 ) {
    if ( a[lo] == key ) {
      return lo;
    } else {
      return(NOT_FOUND);
    }
  } else {
    int mid = (hi+lo)/2;
    if ( key < a[mid] ) {
      return binsea( key, a, lo, mid);
    } else {
      return binsea( key, a, mid, hi);
    }
  }

}


/*

   Notes on a variant on binary search.

   It's given here in a tail-recursive formulation, which is
   clearest, but perhaps not so natural for C.  It's reasonably
   straightforward to convert it to an iterative formulation.  I
   recommend that you try this as an exercise.  The only tricky
   part is to get the re-assignment of variables right in the
   iteration.

   I think you'll appreciate that this algorithm is tighter than
   the usual version.  It relies on several conventions,
   conditions, and observations:

   1. The range of the array lo to hi is inclusive at the lower
      bound but exclusive at the upper bound.  That is, hi
      notionally indexes the array element just past the range
      of elements considered.  (I say "notionally" because hi
      might even point just past the end of the array, as it
      typically will on the initial call.)  This might seem a
      bit unintuitive at first, but is actually neater and
      requires less adjustment of indices.  For example, the
      length of a range is just the difference, (hi-lo), without
      needing any adjustment.  Similarly, when the range gets
      split in half at the midpoint mid, the lower half range
      runs up to mid (exclusive), and the upper half range runs
      from mid (inclusive).

      This is different from the version commonly seen, in which
      the range lo to hi is inclusive at both ends, often
      requiring adjustment by plus or minus one.

      This convention of ranges being inclusive at the lower end
      and exclusive at the upper end is generally applicable,
      and often leads to more elegant formulations of algorthms,
      for example in Quicksort.  That's one of the main reasons
      it was adopted in Common Lisp.

   2. The version commonly seen explicitly checks whether the
      sought value (called "key" here) is found at the midpoint
      at every step.  In contrast, the version given here is
      driven strictly by subdividing the range until it gets
      down to a single element where the key must be if it's in
      the array at all.  Only then is a comparison made with the
      key.

      In general, this version is more efficient, since it has
      fewer operations at each step, with the following caveats:

         (a) If the sought key is very likely to be in the
             array, particularly if it occurs multiple times,
             then the common version stands a good chance of
             stumbling on the key and exiting early, being
             quicker in this case.  However, if the sought key
             is unlikely to be in the array, then this version
             will definitely be quicker, because it never wastes
             time checking for the key until it has to at the
             very end.  Another way of looking at it is in
             information-theoretic terms: If the key-equality
             test succeeds, then it provides a lot of
             information about where the key is, but this will
             happen quite rarely.  If the key-equality test
             fails, then it provides very little information,
             and this will happen most of the time.  So the
             expected information gain is quite small.  In
             contrast, an ordering comparison (in this case
             "<"), reliably provides about one bit of
             information each time, which half the key might lie
             in.  (This provides another insight into the
             complexity: The location of the key in the array
             takes about log n bits to represent in binary
             notation.  Each step provides about one more bit of
             that location, so the algorithm takes order log n
             time overall.)

         (b) Notwithstanding the above, many computers don't
             have explicit equality, less, greater, test
             instructions. Rather they have a "compare"
             instruction, which sets various "condition codes"
             (bits in the CPU), which can be tested by later
             condtional jump instructions.  On such a computer,
             with a good optimizing compiler, the two comparison
             tests of the common algorithm, could be compiled
             into a single comparison instruction (along with
             the conditional jumps), so the two tests in the
             common version would cost only a little more than
             the single test in this version.

         (c) On today's computers, the dominant cost is likely
             to be the time to fetch array elements from memory,
             rather than operations, so in practice the run-time
             difference between the versions is likely to be
             negligible.  Moreover, what differences there are
             in operation counts between the versions are only
             by constant factors, so both have the same
             complexity.  So, really the main advantage of this
             version is in the aesthetics of its elegance.

   3. This version requires the preconditions that the initial
      range searched be non-empty, that is hi>lo, and that
      indices be non-negative.  If that can't be assured, then
      it'll be necessary to wrap this in an outer routine to
      check for the preconditions and return NOT_FOUND if they
      aren't true.  There is a little subtlety here.  If the
      initial range is non-empty, then because of the way the
      midpoint is computed (with the range conventions), then
      empty subranges can never appear.  So the base case need
      only be to test for a range of size 1, that is (hi-lo ==
      1).

   4. Since C as a language does not have proper nested block
      structure of functions, all arguments have to be passed to
      binsea on each call, even though only the lo and hi
      arguments change.  In a language with proper block
      structure, the main work could be done by a nested
      recursive function taking only lo and hi as arguments, the
      other arguments, key and a[], being accessed as what are
      sometimes called "hung variables".

   5. Often a useful way of thinking about iterative or
      recursive algorithms is in terms of some invariant
      condition.  In this case the useful invariant condition is
      something like: "If the key is in the array at all, then
      it lies in the (non-empty) range [lo,hi)."  (This uses the
      mathematical convention for ranges inclusive at one end,
      and exclusive at the other.)  We use the comparison
      against the element at the midpoint to find a smaller
      (half-size) range for which the invariant must hold.  When
      we get down to a single-element range, then it's trivial
      to test whether the key is at that position.

   6. The analysis here is based on the assumption that all the
      elements in the array are distinct, that is, no
      duplicates.  I haven't fully considered what happens if
      there are multiple occurrences of the key in the array.
      If there are multiple occurrences, then they must occur in
      a contiguous run (since the array is sorted).  I think in
      this case the algorithm presented here will find the
      occurrence at the upper end of that run, but, like I said,
      I haven't fully analysed this case.  If there can be
      multiple occurrences, then it'd be an interesting exercise
      to develop an elegant algorithm, based on binary search,
      which will find the full range of occurrences, not just
      one point at which the key occurs.

   7. In my version here, the midpoint mid is computed in the
      obvious way as (lo+hi)/2.  However, you can make a strong
      case for computing it instead as mid=lo+(hi-lo)/2.  Why
      this extra complication?  Suppose you're dealing with a
      *really* big array, gigabytes in size.  Then at some point
      in the computation, you might get to a stage where both lo
      and hi are really big integers, so that the sum (lo+hi)
      overflows the range of representable integers.  In many
      programming language implementations this overflow is
      silent (not caught) and you just end up with a nonsensical
      result.  If you're using twos-complement signed integers
      (typical of ints in languages like C, C++, Java), then
      you'll likely get a nonsensical negative result.  Bad
      things can happen.  If you have array bounds checking,
      then that negative index will be trapped somehow, and your
      program might just abort with an error.  If you don't have
      array bounds checking, then your program will likely just
      index into a section of memory outside the array, silently
      producing wrong results.  This is the sort of bug that
      attackers can exploit, by feeding in a data-set contrived
      to trigger the bug.  Of course there are work-arounds,
      like computing (lo+hi)/2 in higher-precision arithmetic,
      but tweaking the computation as indicated above is
      probably the simplest and best solution.  And it may be
      that in your application you know that you'll never be
      using really big arrays, but then you never know how other
      people will mis-use your code in future.

   8. Psychology: There's some psychology of programming
      involved here.  Since we conceive of binary search as
      being to find the key in the array, there's a natural
      tendency, once we've computed mid, to check immediately
      whether the key is at a[mid].  But as we've seen from the
      above discussion, in many cases this is a waste of time,
      and it's better to concentrate just on reducing the
      interval.  If we'd conceived of the problem as trying to
      prove that the key isn't in the array, then maybe we
      would've approached it differently.  I guess my point is
      that how we mentally conceive of a problem can
      unconsciously affect how we approach it.

*/
