val scala3Version = "3.2.2"
val http4sVersion = "0.23.14"
val circeVersion = "0.14.5"

lazy val root = project
  .enablePlugins(GraalVMNativeImagePlugin, DockerPlugin, JavaServerAppPackaging)
  .in(file("."))
  .settings(
    name := "Scala3 Cats http4s starter",
    version := "0.1.0-SNAPSHOT",

    scalaVersion := scala3Version,

    libraryDependencies ++= Seq(
      // HTTP server
      "org.http4s" %% "http4s-core" % http4sVersion,
      "org.http4s" %% "http4s-dsl" % http4sVersion,
      "org.http4s" %% "http4s-blaze-server" % http4sVersion,
      "org.http4s" %% "http4s-blaze-client" % http4sVersion,
      "org.http4s" %% "http4s-circe" % http4sVersion,

      // JSON
      "io.circe" %% "circe-core" % circeVersion,
      "io.circe" %% "circe-generic" % circeVersion,
      "io.circe" %% "circe-config" % "0.10.0",

      // Config
      "com.typesafe" % "config" % "1.4.2",
    ),

    libraryDependencies ++= Seq(
      "org.scalameta" %% "munit" % "0.7.29" % Test,
      "org.typelevel" %% "munit-cats-effect-3" % "1.0.7" % Test,
    ),

    // Docker packaging:
    Docker / packageName := packageName.value,
    Docker / version := version.value,
    dockerBaseImage := "openjdk:17",
    dockerExposedPorts ++= Seq(8080),

    // GraalVM packaging:
    assembly / mainClass := Some("sw.Main"),
    assembly / assemblyJarName := s"${packageName.value}_3-${version.value}.jar",
    graalVMNativeImageGraalVersion := Some("22.3.1"),

    graalVMNativeImageOptions ++= Seq(
      "--no-fallback",
      "--static",
      "--allow-incomplete-classpath",
      "--report-unsupported-elements-at-runtime",
      "--enable-https",
      "--enable-http",
      "--enable-all-security-services",
      "-H:+ReportExceptionStackTraces",
    ),

    scalacOptions ++= Seq(
      "-feature", "-unchecked", "-deprecation", "-encoding", "utf8",
      "-Xfatal-warnings"
    ),

  )
