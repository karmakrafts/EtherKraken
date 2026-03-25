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

import dev.karmakrafts.conventions.asAAR
import dev.karmakrafts.conventions.configureJava
import dev.karmakrafts.conventions.dokka.configureDokka
import dev.karmakrafts.conventions.kotlin.defaultCompilerOptions
import dev.karmakrafts.conventions.kotlin.withLinux
import dev.karmakrafts.conventions.setProjectInfo
import org.jetbrains.kotlin.gradle.ExperimentalKotlinGradlePluginApi
import org.jetbrains.kotlin.gradle.plugin.mpp.KotlinNativeTarget

plugins {
    alias(libs.plugins.kotlin.multiplatform)
    alias(libs.plugins.dokka)
    `maven-publish`
    signing
}

configureJava(rootProject.libs.versions.java)

configureDokka {
    withJava()
    withKotlin()
}

kotlin {
    defaultCompilerOptions()
    withSourcesJar()
    withLinux()
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