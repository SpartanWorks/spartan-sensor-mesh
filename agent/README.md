## Scala 3, Cats Effect, Http4s starter project

This is meant to be a simple project skeleton that can be easily turned into something else. It was written with ChatGPT assist.

The only thing it does is defining an API endpoint that translates from https://freecurrencyapi.com/ format to an arbitrary another format, but including validation, etc, etc. If you wish to run it to see how it works, then you'll need to grab an API key from Free Currency API and then run the app as follows: `FREE_CURRENCY_API_KEY=<the-api-key> sbt run`.


### Usage

This is a normal sbt project. You can compile code with `sbt compile`, run it with `sbt run`, and `sbt console` will start a Scala 3 REPL.

Packaging is done via `sbt Docker/publishLocal` or, to build a native image for GraalVM, `sbt GraalVMNativeImage/packageBin`.

There's also `sbt scalafmt`, `sbt scalafixAll` and `sbt coverage test coverageReport` available.
