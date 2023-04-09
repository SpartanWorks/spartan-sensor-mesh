#! /bin/bash

PACKAGE=scala3-cats-http4s-starter_3
VERSION=0.1.0-SNAPSHOT
SCALA_VERSION=3.2.2
META_INF_DIR=./src/main/resources/META-INF/native-image

echo "Make sure to run `sbt assembly` first."
echo "Once started, make some use of the app for a while, and then close it."

java -agentlib:native-image-agent=config-output-dir="${META_INF_DIR}" -jar ./target/scala-"${SCALA_VERSION}"/"${PACKAGE}"-"${VERSION}".jar

# NOTE After running this script you are supposed to use the app for a while. It'll generate a few files and place them in:
#
# .
#  src
#    main
#      resources
#        META-INF/native-image
#          jni-config.json
#          proxy-config.json
#          reflect-config.json
#          resource-config.json
#          serialization-config.json
#
# These files are used for instrumenting GraalVM dynamic feature compilation.
