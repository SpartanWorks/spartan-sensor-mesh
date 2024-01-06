import com.typesafe.sbt.packager.docker._

val scala3Version = "3.2.2"
val http4sVersion = "0.23.14"
val circeVersion = "0.14.5"

lazy val root = project
  .enablePlugins(GraalVMNativeImagePlugin, DockerPlugin, JavaServerAppPackaging)
  .in(file("."))
  .settings(
    name := "Spartan Sensor Mesh",
    version := "0.1.0-SNAPSHOT",

    scalaVersion := scala3Version,

    resolvers += Resolver.githubPackages("SpartanWorks"),
    libraryDependencies ++= Seq(
      // Basics
      "org.typelevel" %% "cats-effect" % "3.5.0",
      "co.fs2" %% "fs2-core" % "3.7.0",
      "co.fs2" %% "fs2-io" % "3.7.0",

      // Domain
      "spartan.works" %% "ssm-client" % "0.0.4",
      "fr.davit" %% "scout" % "0.2.1",

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

      // Misc
      "com.typesafe" % "config" % "1.4.2",
      "ch.qos.logback" % "logback-classic" % "1.4.6",
      "org.typelevel" %% "log4cats-core" % "2.5.0",
      "org.typelevel" %% "log4cats-slf4j" % "2.5.0",

      // Test
      "org.scalameta" %% "munit" % "0.7.29" % Test,
      "org.typelevel" %% "munit-cats-effect-3" % "1.0.7" % Test,
    ),

    // Docker packaging:
    Docker / packageName := packageName.value,
    Docker / version := version.value,
    dockerBaseImage := "openjdk:23-slim",
    dockerCommands := dockerCommands.value.flatMap {
      case cmd @ ExecCmd("VOLUME", "/opt/docker/data") =>
        Seq(cmd, ExecCmd("RUN", "apt-get", "update"), ExecCmd("RUN", "apt-get", "install", "-y", "nut-client"))
      case cmd => Seq(cmd)
    },
    dockerExposedVolumes := Seq("/opt/docker/data"),
    dockerEntrypoint := Seq("/opt/docker/bin/spartan-sensor-mesh", "/opt/docker/data/agent_config.json"),
    dockerExposedPorts ++= Seq(8080),

    // Uberjar
    assembly / mainClass := Some("ssm.Main"),
    assembly / assemblyJarName := s"${packageName.value}_3-${version.value}.jar",
    assemblyMergeStrategy := {
      case PathList(ps @ _*) if ps.last.endsWith("module-info.class") => MergeStrategy.first
      case x => (assemblyMergeStrategy).value(x)
    },

    // GraalVM packaging:
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

    // Publishing:
    githubOwner := "SpartanWorks",
    githubRepository := "spartan-sensor-mesh",
    githubTokenSource := TokenSource.Or(
      TokenSource.Environment("GITHUB_TOKEN"),
      TokenSource.Environment("SHELL"), // NOTE To get around the annoying error when developing locally.
    ),

    // Misc:
    scalacOptions ++= Seq(
      "-feature", "-unchecked", "-deprecation", "-encoding", "utf8",
      "-Xfatal-warnings"
    ),

  )

val runTraced = taskKey[Unit]("Runs the project in native-image tracing mode. Will create a bunch of configuration files for the dynamic features.")

runTraced := {
  val curState = state.value
  val resources = (Compile / resourceDirectory).value
  val updState = Project.extract(curState).appendWithoutSession(
    Seq(
      Compile / run / fork := true,
      Compile / run / javaOptions += s"-agentlib:native-image-agent=config-output-dir=${resources}/META-INF/native-image"
    ),
    curState
  )
  Project.extract(updState).runInputTask(Compile / run, "", updState)
}
