# fizz-buzz
Dissecting the most difficult trivial programming puzzle

The [FizzBuzz](https://en.wikipedia.org/wiki/Fizz_buzz#Programming) problem is said to be notoriously trivial, but it is __not__. Read the following (in the given order) to understand why:

- [fizzbuzz.pdf](fizzbuzz.pdf) - Originally published in the [Monad.Reader](https://themonadreader.wordpress.com/) magazine :broken_heart:. It describes why the problem is actually an instance of a difficult programming pattern that seems to require either code duplication, information-flow vs. control-flow mismatch, or higher-order functions. Of course, we choose the latter.

- [fizzbuzz.c](fizzbuzz.c) - A C implementation of the solution proposed in the paper above. (Yes, if you look from the right angle, C is a dependently-typed functional language.)

- [fizzbuzz-semigroup.md](fizzbuzz-semigroup.md) - If you're inner pedant is still not satisfied, let's ditch the empty list.

## Ports

The solution from the Monad.Reader paper has been translated to many different languages by a number of programming languages enthusiasts. For example:

- [To Haskell](https://github.com/mathiasverraes/skiphaltprint) by [Mathias Verraes](https://github.com/mathiasverraes) (the original implementation in the paper was actually in Haskell, but here the derivation is "animated" through file history)

- [To JavaScript](https://stackoverflow.com/a/55229067) by [QuentinUK](https://stackoverflow.com/users/832009/quentinuk)

- [To PureScript](https://gist.github.com/matthewleon/77951008507fb6d0904c3c9bf29eade8) by [Matthew Leon Grinshpun](https://github.com/matthewleon)

- [To Go](https://github.com/orirawlings/fizzbuzz) by [Ori Rawlings](https://github.com/orirawlings)

- [To F#](http://www.fssnip.net/mX/title/FizzBuzz-in-F-by-Embedding-a-DomainSpecific-Language) by [Cesar Mendoza](http://www.fssnip.net/authors/Cesar+Mendoza)
