#include <gtest/gtest.h>
#include "ui/updatesdialog.h"
#include <QApplication>

namespace pboman3::ui::test {
    TEST(SemanticVersionTest, Ctor_Initializes_Without_Params) {
        const SemanticVersion ver;
        ASSERT_FALSE(ver.isValid());
        ASSERT_EQ("", ver.raw());
    }

    class CtorWithParamTest1 : public testing::TestWithParam<QString> {
    };

    TEST_P(CtorWithParamTest1, Ctor_Initializes_Valid_Version) {
        const SemanticVersion ver(GetParam());
        ASSERT_TRUE(ver.isValid());
        ASSERT_EQ(GetParam(), ver.raw());
    }

    INSTANTIATE_TEST_SUITE_P(SemanticVersionTest, CtorWithParamTest1, testing::Values(
                                 QString("1.0.0"),
                                 QString("1.1.0"),
                                 QString("1.1.1"),
                                 QString("1.1.1-alpha"),
                                 QString("0.0.1-alpha")
                             ));

    class CtorWithParamTest2 : public testing::TestWithParam<QString> {
    };

    TEST_P(CtorWithParamTest2, Ctor_Initializes_Valid_Version) {
        const SemanticVersion ver(GetParam());
        ASSERT_FALSE(ver.isValid());
        ASSERT_EQ(GetParam(), ver.raw());
    }

    INSTANTIATE_TEST_SUITE_P(SemanticVersionTest, CtorWithParamTest2, testing::Values(
                                 QString("0.0.0"),
                                 QString("1.1.a"),
                                 QString("some text")
                             ));

    struct OperatorTestParam {
        QString ver1;
        QString ver2;
        bool expectedResult;
    };

    class OperatorLessTest : public testing::TestWithParam<OperatorTestParam> {
    };

    TEST_P(OperatorLessTest, Operator_Less_Functional) {
        const SemanticVersion ver1(GetParam().ver1);
        const SemanticVersion ver2(GetParam().ver2);
        ASSERT_EQ(GetParam().expectedResult, ver1 < ver2);
    }

    INSTANTIATE_TEST_SUITE_P(SemanticVersionTest, OperatorLessTest, testing::Values(
                                 OperatorTestParam{ "0.0.1", "0.0.2", true },
                                 OperatorTestParam{ "0.1.0", "0.2.0", true },
                                 OperatorTestParam{ "1.0.0", "2.0.0", true },
                                 OperatorTestParam{ "1.0.0", "1.1.0", true },
                                 OperatorTestParam{ "1.0.0", "1.0.1", true },
                                 OperatorTestParam{ "0.1.0", "0.1.1", true },

                                 OperatorTestParam{ "0.0.2", "0.0.1", false },
                                 OperatorTestParam{ "0.2.0", "0.1.0", false },
                                 OperatorTestParam{ "2.0.0", "1.0.0", false },
                                 OperatorTestParam{ "1.1.0", "1.0.0", false },
                                 OperatorTestParam{ "1.0.1", "1.0.0", false },
                                 OperatorTestParam{ "0.1.1", "0.1.0", false }
                             ));

    TEST(GithubLatestVersionTest, Returns_Valid_Version) {
        GithubLatestVersion githubVersion;

        int argc = 0;
        char argv[1];
        QApplication app(argc, reinterpret_cast<char**>(argv));
        QEventLoop loop;
        
        bool successCalled = false;
        QObject::connect(&githubVersion, &GithubLatestVersion::success, [&successCalled](const SemanticVersion& ver) {
            successCalled = true;
            ASSERT_TRUE(ver.isValid());
        });
        
        QObject::connect(&githubVersion, &GithubLatestVersion::success, &loop, &QEventLoop::quit);
        QObject::connect(&githubVersion, &GithubLatestVersion::error, &loop, &QEventLoop::quit);
        githubVersion.check();
        loop.exec();
        
        ASSERT_TRUE(successCalled);
    }
}
