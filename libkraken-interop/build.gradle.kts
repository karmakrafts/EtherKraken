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
import org.jetbrains.kotlin.gradle.ExperimentalKotlinGradlePluginApi

plugins {
    alias(libs.plugins.kotlin.multiplatform)
    alias(libs.plugins.dokka)
    `maven-publish`
    signing
}

configureJava(rootProject.libs.versions.java)

val debugLibKraken: Boolean = System.getProperty("libkraken.build.debug") == "true"

val compileLibKraken: TaskProvider<Exec> = tasks.register("compileLibKraken", Exec::class) {
    group = "interop"
    inputs.dir(rootDir.resolve("libkraken").resolve("include"))
    inputs.dir(rootDir.resolve("libkraken").resolve("src"))
    val suffix = if (debugLibKraken) "debug" else "release"
    outputs.dir(rootDir.resolve("libkraken").resolve("build-$suffix"))
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
                    val suffix = if (debugLibKraken) "_debug" else ""
                    create("libkraken$suffix") {
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