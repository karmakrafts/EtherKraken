/*
 * Copyright 2026 Karma Krafts
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

@file:OptIn(ExperimentalKotlinGradlePluginApi::class)

import dev.karmakrafts.conventions.configureJava
import dev.karmakrafts.conventions.kotlin.defaultCompilerOptions
import dev.karmakrafts.conventions.setProjectInfo
import org.apache.tools.ant.taskdefs.condition.Os
import org.jetbrains.kotlin.gradle.ExperimentalKotlinGradlePluginApi

plugins {
    alias(libs.plugins.kotlin.multiplatform)
    alias(libs.plugins.dokka)
    `maven-publish`
    signing
}

check(Os.isFamily(Os.FAMILY_UNIX)) { "libkraken interop module requires Linux host to build" }
configureJava(rootProject.libs.versions.java)

val compileLibKraken = tasks.register("compileLibKraken", Exec::class) {
    group = "interop"
    inputs.dir(rootDir.resolve("libkraken").resolve("include"))
    inputs.dir(rootDir.resolve("libkraken").resolve("src"))
    outputs.dir(rootDir.resolve("libkraken").resolve("build-release"))
    workingDir = rootDir.resolve("libkraken")
    commandLine("/bin/bash", "build.sh")
}

kotlin {
    defaultCompilerOptions()
    withSourcesJar()
    linuxArm64 {
        compilations {
            named("main") {
                cinterops {
                    create("libkraken") {
                        tasks.named(interopProcessingTaskName) {
                            dependsOn(compileLibKraken)
                        }
                    }
                }
            }
        }
    }
    applyDefaultHierarchyTemplate()
    sourceSets {
        commonTest {
            dependencies {
                implementation(libs.kotlin.test)
            }
        }
    }
}

publishing {
    setProjectInfo(
        name = "libkraken Interop",
        description = "CInterop module for the libkraken HAL",
        url = "https://git.karmakrafts.dev/kk/hardware-projects/etherkraken"
    )
}