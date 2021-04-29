// Copyright 2021 The WebNN-native Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "src/tests/WebnnTest.h"

class SubTests : public WebnnTest {};

TEST_F(SubTests, SubTwoInputs) {
    const ml::GraphBuilder builder = ml::CreateGraphBuilder(GetContext());
    const ml::Operand a = utils::BuildInput(builder, "a", {3, 4, 5});
    const ml::Operand b = utils::BuildInput(builder, "b", {3, 4, 5});
    const ml::Operand c = builder.Sub(a, b);
    const ml::Graph graph = utils::AwaitBuild(builder, {{"c", c}});
    ASSERT_TRUE(graph);
    const std::vector<float> dataA = {
        1.7640524,   0.4001572,   0.978738,   2.2408931,   1.867558,    -0.9772779,  0.95008844,
        -0.1513572,  -0.10321885, 0.41059852, 0.14404356,  1.4542735,   0.7610377,   0.12167501,
        0.44386324,  0.33367434,  1.4940791,  -0.20515826, 0.3130677,   -0.85409576, -2.5529897,
        0.6536186,   0.8644362,   -0.742165,  2.2697546,   -1.4543657,  0.04575852,  -0.18718386,
        1.5327792,   1.4693588,   0.15494743, 0.37816253,  -0.88778573, -1.9807965,  -0.34791216,
        0.15634897,  1.2302907,   1.2023798,  -0.3873268,  -0.30230275, -1.048553,   -1.420018,
        -1.7062702,  1.9507754,   -0.5096522, -0.4380743,  -1.2527953,  0.7774904,   -1.6138978,
        -0.21274029, -0.89546657, 0.3869025,  -0.51080513, -1.1806322,  -0.02818223, 0.42833188,
        0.06651722,  0.3024719,   -0.6343221, -0.36274117,
    };
    const ml::Input inputA = {dataA.data(), dataA.size() * sizeof(float)};
    const std::vector<float> dataB = {
        -0.67246044, -0.35955316, -0.8131463,  -1.7262826,  0.17742614,  -0.40178093, -1.6301984,
        0.46278226,  -0.9072984,  0.0519454,   0.7290906,   0.12898292,  1.1394007,   -1.2348258,
        0.40234163,  -0.6848101,  -0.87079716, -0.5788497,  -0.31155252, 0.05616534,  -1.1651498,
        0.9008265,   0.46566245,  -1.5362437,  1.4882522,   1.8958892,   1.1787796,   -0.17992483,
        -1.0707526,  1.0544517,   -0.40317693, 1.222445,    0.20827498,  0.97663903,  0.3563664,
        0.7065732,   0.01050002,  1.7858706,   0.12691209,  0.40198937,  1.8831507,   -1.347759,
        -1.270485,   0.9693967,   -1.1731234,  1.9436212,   -0.41361898, -0.7474548,  1.922942,
        1.4805148,   1.867559,    0.90604466,  -0.86122566, 1.9100649,   -0.26800337, 0.8024564,
        0.947252,    -0.15501009, 0.61407936,  0.9222067};
    const ml::Input inputB = {dataB.data(), dataB.size() * sizeof(float)};
    const ml::Result result = utils::AwaitCompute(graph, {{"a", inputA}, {"b", inputB}}).Get("c");
    EXPECT_TRUE(utils::CheckShape(result, {3, 4, 5}));
    const std::vector<float> expectedValue(
        {2.436513,    0.7597104,  1.7918843,   3.9671757,  1.6901319,   -0.5754969, 2.5802867,
         -0.61413944, 0.80407953, 0.35865313,  -0.585047,  1.3252906,   -0.378363,  1.3565009,
         0.04152161,  1.0184845,  2.3648763,   0.3736914,  0.6246202,   -0.9102611, -1.3878399,
         -0.24720794, 0.39877376, 0.79407865,  0.7815025,  -3.350255,   -1.1330211, -0.00725903,
         2.6035318,   0.4149071,  0.55812436,  -0.8442825, -1.0960608,  -2.9574356, -0.7042786,
         -0.5502242,  1.2197906,  -0.5834907,  -0.5142389, -0.7042921,  -2.9317036, -0.07225895,
         -0.43578517, 0.9813787,  0.66347116,  -2.3816955, -0.83917636, 1.5249453,  -3.53684,
         -1.6932551,  -2.7630255, -0.51914215, 0.35042053, -3.0906973,  0.23982115, -0.3741245,
         -0.88073474, 0.45748198, -1.2484014,  -1.2849479});
    EXPECT_TRUE(utils::CheckValue(result, expectedValue));
}

TEST_F(SubTests, SubBroadcast) {
    const ml::GraphBuilder builder = ml::CreateGraphBuilder(GetContext());
    const ml::Operand a = utils::BuildInput(builder, "a", {3, 4, 5});
    const ml::Operand b = utils::BuildInput(builder, "b", {5});
    const ml::Operand c = builder.Sub(a, b);
    const ml::Graph graph = utils::AwaitBuild(builder, {{"c", c}});
    ASSERT_TRUE(graph);
    const std::vector<float> dataA = {
        0.37642553,  -1.0994008, 0.2982382,  1.3263859,   -0.69456786, -0.14963454, -0.43515354,
        1.8492638,   0.67229474, 0.40746182, -0.76991606, 0.5392492,   -0.6743327,  0.03183056,
        -0.6358461,  0.67643327, 0.57659084, -0.20829876, 0.3960067,   -1.0930616,  -1.4912575,
        0.4393917,   0.1666735,  0.63503146, 2.3831449,   0.94447947,  -0.91282225, 1.1170163,
        -1.3159074,  -0.4615846, -0.0682416, 1.7133427,   -0.74475485, -0.82643855, -0.09845252,
        -0.6634783,  1.1266359,  -1.0799315, -1.1474687,  -0.43782005, -0.49803245, 1.929532,
        0.9494208,   0.08755124, -1.2254355, 0.844363,    -1.0002153,  -1.5447711,  1.1880298,
        0.3169426,   0.9208588,  0.31872764, 0.8568306,   -0.6510256,  -1.0342429,  0.6815945,
        -0.80340964, -0.6895498, -0.4555325, 0.01747916};
    const ml::Input inputA = {dataA.data(), dataA.size() * sizeof(float)};
    const std::vector<float> dataB = {-0.35399392, -1.3749512, -0.6436184, -2.2234032, 0.62523144};
    const ml::Input inputB = {dataB.data(), dataB.size() * sizeof(float)};
    const ml::Result result = utils::AwaitCompute(graph, {{"a", inputA}, {"b", inputB}}).Get("c");
    EXPECT_TRUE(utils::CheckShape(result, {3, 4, 5}));
    const std::vector<float> expectedValue({
        0.73041946,  0.27555048, 0.9418566,   3.549789,    -1.3197993,  0.20435938,  0.9397977,
        2.4928823,   2.895698,   -0.21776962, -0.41592214, 1.9142004,   -0.03071427, 2.2552338,
        -1.2610775,  1.0304272,  1.9515421,   0.43531966,  2.61941,     -1.718293,   -1.1372637,
        1.814343,    0.8102919,  2.8584347,   1.7579134,   1.2984734,   0.462129,    1.7606347,
        0.90749586,  -1.0868161, 0.28575233,  3.088294,    -0.10113645, 1.3969647,   -0.72368395,
        -0.3094844,  2.5015872,  -0.4363131,  1.0759345,   -1.0630515,  -0.14403853, 3.3044834,
        1.5930393,   2.3109546,  -1.850667,   1.1983569,   0.37473595,  -0.9011527,  3.411433,
        -0.30828884, 1.2748528,  1.6936789,   1.500449,    1.5723777,   -1.6594744,  1.0355884,
        0.5715416,   -0.0459314, 1.7678707,   -0.60775226,
    });
    EXPECT_TRUE(utils::CheckValue(result, expectedValue));
}
