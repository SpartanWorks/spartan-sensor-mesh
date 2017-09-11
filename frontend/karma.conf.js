"use strict";

const gulp = require("./gulpfile.js");

process.env.TEST = true;

module.exports = (karma) => {
  karma.set({

    frameworks: [
      "browserify",
      "jasmine",
    ],

    files: ["src/**/*.spec.ts?(x)"],

    preprocessors: {
      "src/**/*.ts?(x)": ["browserify"]
    },

    browserify: {
      debug: true,
      plugin: [
        "tsify",
        ["css-modulesify", {
          before: gulp.postcss,
        }],
      ],
    },

    mime: {
      "text/x-typescript": ["ts", "tsx"],
    },

    reporters: ["progress"],

    customLaunchers: {
      HeadlessChrome: {
        base: "Chrome",
        flags: [
          "--use-fake-device-for-media-stream", "--use-fake-ui-for-media-stream",
          "--user-data-dir=/tmp/data", "--headless", "--disable-gpu",
          "--remote-debugging-address=0.0.0.0", "--remote-debugging-port=9222",
        ],
      },
    },

    port: 9999,
    colors: true,
    logLevel: karma.LOG_INFO,
    autoWatch: true,
    browsers: [
      "Chrome",
      "Firefox",
    ],
    captureTimeout: 6000,
  });
};
