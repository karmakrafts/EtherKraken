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
import org.jetbrains.kotlin.gradle.ExperimentalKotlinGradlePluginApi

plugins {
    alias(libs.plugins.kotlin.multiplatform)
}

configureJava(rootProject.libs.versions.java)

kotlin {
    defaultCompilerOptions()
    withSourcesJar()
    linuxArm64 {
        binaries {
            executable {
                entryPoint = "$group.selftest.main"
            }
        }
    }
    applyDefaultHierarchyTemplate()
    sourceSets {
        commonMain {
            dependencies {
                implementation(projects.etherkrakenCore)
            }
        }
        commonTest {
            dependencies {
                implementation(libs.kotlin.test)
            }
        }
    }
}