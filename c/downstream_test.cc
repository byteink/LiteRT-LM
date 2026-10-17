// Copyright 2026 The ODML Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <atomic>
#include <memory>
#include <string>

#include "c/engine.h"

namespace litert::lm::internal {
extern std::atomic<void*> g_metal_device_override;
extern std::atomic<void*> g_metal_command_queue_override;
}  // namespace litert::lm::internal

namespace {

TEST(DownstreamCTest, MetalOverridesCanBeSetAndCleared) {
  int device = 0;
  int queue = 0;
  litert_lm_set_metal_device(&device);
  litert_lm_set_metal_command_queue(&queue);
  EXPECT_EQ(litert::lm::internal::g_metal_device_override.load(), &device);
  EXPECT_EQ(litert::lm::internal::g_metal_command_queue_override.load(),
            &queue);
  litert_lm_set_metal_device(nullptr);
  litert_lm_set_metal_command_queue(nullptr);
  EXPECT_EQ(litert::lm::internal::g_metal_device_override.load(), nullptr);
  EXPECT_EQ(litert::lm::internal::g_metal_command_queue_override.load(),
            nullptr);
}

TEST(DownstreamCTest, CountTokensRejectsNullEngine) {
  EXPECT_EQ(litert_lm_engine_count_tokens(nullptr, "hello"), -1);
  EXPECT_EQ(litert_lm_engine_count_tokens(nullptr, nullptr), -1);
}

TEST(DownstreamCTest, CountTokensMatchesPublicTokenizer) {
  const std::string path = std::string(::testing::SrcDir()) +
                           "/litert_lm/runtime/testdata/test_lm.litertlm";
  LiteRtLmEngineSettings* raw_settings = nullptr;
  ASSERT_EQ(litert_lm_engine_settings_create(path.c_str(), "cpu", nullptr,
                                             nullptr, &raw_settings),
            kLiteRtLmStatusOk);
  std::unique_ptr<LiteRtLmEngineSettings,
                  decltype(&litert_lm_engine_settings_delete)>
      settings(raw_settings, litert_lm_engine_settings_delete);
  LiteRtLmEngine* raw_engine = nullptr;
  ASSERT_EQ(litert_lm_engine_create(settings.get(), &raw_engine),
            kLiteRtLmStatusOk);
  std::unique_ptr<LiteRtLmEngine, decltype(&litert_lm_engine_delete)> engine(
      raw_engine, litert_lm_engine_delete);
  EXPECT_EQ(litert_lm_engine_count_tokens(engine.get(), nullptr), -1);
  for (const char* text : {"", "hello world", "مرحبا بالعالم"}) {
    LiteRtLmTokenizeResult* raw_result = nullptr;
    ASSERT_EQ(litert_lm_engine_tokenize(engine.get(), text, &raw_result),
              kLiteRtLmStatusOk);
    std::unique_ptr<LiteRtLmTokenizeResult,
                    decltype(&litert_lm_tokenize_result_delete)>
        result(raw_result, litert_lm_tokenize_result_delete);
    size_t count = 0;
    ASSERT_EQ(litert_lm_tokenize_result_get_num_tokens(result.get(), &count),
              kLiteRtLmStatusOk);
    EXPECT_EQ(litert_lm_engine_count_tokens(engine.get(), text), count);
  }
}

}  // namespace
