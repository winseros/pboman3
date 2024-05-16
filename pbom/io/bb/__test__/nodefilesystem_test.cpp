#include "io/bb/nodefilesystem.h"
#include "exception.h"
#include <QTemporaryDir>
#include <gtest/gtest.h>

namespace pboman3::io::test {
    TEST(NodeFileSystemTest, AllocatePath_Creates_And_Sanitizes_Path_For_Node) {
        const QTemporaryDir dir;
        const NodeFileSystem fs(QDir(dir.path()));

        PboNode root("root", PboNodeType::Container, nullptr);
        const PboNode* file = root.createHierarchy(PboPath("e1\t/e2\t/e3\t.txt"));

        const QString path = fs.allocatePath(file);
        const QString expected = dir.path() + "/e1%9/e2%9/e3%9.txt";

        ASSERT_EQ(expected, path);

        const QFileInfo f(path);
        ASSERT_TRUE(f.dir().exists());
    }

    TEST(NodeFileSystemTest, AllocatePath_Relative_Creates_And_Sanitizes_Path_For_Node) {
        const QTemporaryDir dir;
        const NodeFileSystem fs(QDir(dir.path()));

        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode f1("f1", PboNodeType::Folder, &root);
        const PboNode* file = f1.createHierarchy(PboPath("e1\t/e2\t/e3\t.txt"));

        const QString path = fs.allocatePath(&f1, file);
        const QString expected = dir.path() + "/e1%9/e2%9/e3%9.txt";

        ASSERT_EQ(expected, path);

        const QFileInfo f(path);
        ASSERT_TRUE(f.dir().exists());
    }

    TEST(NodeFileSystemTest, AllocatePath_Throws_If_Child_Does_Not_Belong_To_Parent) {
        const QTemporaryDir dir;
        const NodeFileSystem fs(QDir(dir.path()));

        PboNode root("root", PboNodeType::Container, nullptr);
        const PboNode* file = root.createHierarchy(PboPath("e1/e2/e3.txt"));

        ASSERT_THROW(fs.allocatePath(file, file->parentNode()), InvalidOperationException);
    }

    TEST(NodeFileSystemTest, ComposeAbsolutePath_Returns_Sanitized_Path) {
        const QTemporaryDir dir;
        const NodeFileSystem fs(QDir(dir.path()));

        PboNode root("root", PboNodeType::Container, nullptr);
        const PboNode* file = root.createHierarchy(PboPath("e1\t/e2\t/e3\t.txt"));

        const QString path = fs.composeAbsolutePath(file);
        const QString expected = dir.path() + QDir::separator() + "e1%9" + QDir::separator() + "e2%9" + QDir::separator() +
            "e3%9.txt";

        ASSERT_EQ(expected, path);

        const QFileInfo f(path);
        ASSERT_FALSE(f.dir().exists());
    }

    TEST(NodeFileSystemTest, ComposeRelativePath_Returns_Sanitized_Path) {
        const QTemporaryDir dir;
        const NodeFileSystem fs(QDir(dir.path()));

        PboNode root("root", PboNodeType::Container, nullptr);
        const PboNode* file = root.createHierarchy(PboPath("e1\t/e2\t/e3\t.txt"));

        const QString path = fs.composeRelativePath(file);
        const QString expected = QString("e1%9") + QDir::separator() + "e2%9" + QDir::separator() + "e3%9.txt";

        ASSERT_EQ(expected, path);
    }
}
