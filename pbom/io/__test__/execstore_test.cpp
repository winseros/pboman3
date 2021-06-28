#include <gtest/gtest.h>
#include "io/execstore.h"
#include <QDir>
#include <QTemporaryFile>
#include <QUuid>
#include "io/bs/fsrawbinarysource.h"
#include <Windows.h>
#include <fileapi.h>

namespace pboman3::test {
    TEST(ExecStoreTest, ExecSync_Extracts_New_File) {
        //dummy files
        QTemporaryFile f1;
        f1.open();
        f1.write(QByteArray("some text data 1"));
        f1.close();

        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1/file1.txt"));

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f1.fileName()));
        e1->binarySource->open();

        //the object tested
        const QString name = QDir::tempPath() + QDir::separator() + "test_"
            + QUuid::createUuid().toString(QUuid::WithoutBraces);
        ExecStore store(name);
        const QString sync = store.execSync(e1, []() { return false; });

        //check the result
        //c:\users\%username%\temp\<name>\<guid>\e1\file1.txt
        ASSERT_TRUE(sync.startsWith(QFileInfo(name).absoluteFilePath()));
        ASSERT_TRUE(sync.endsWith("e1/file1.txt"));

        //ensure the files have their content
        QFile c1(sync);
        c1.open(QIODeviceBase::ReadOnly);
        ASSERT_EQ(c1.readAll(), "some text data 1");
        c1.close();
    }

    TEST(ExecStoreTest, ExecSync_Reuses_Extracted_File) {
        //dummy files
        QTemporaryFile f1;
        f1.open();
        f1.write(QByteArray("some text data 1"));
        f1.close();

        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1/file1.txt"));

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f1.fileName()));
        e1->binarySource->open();

        //the object tested
        const QString name = QDir::tempPath() + QDir::separator() + "test_"
            + QUuid::createUuid().toString(QUuid::WithoutBraces);
        ExecStore store(name);
        const QString sync1 = store.execSync(e1, []() { return false; });
        const QString sync2 = store.execSync(e1, []() { return false; });

        //check the result
        ASSERT_EQ(sync1, sync2);
    }

    TEST(ExecStoreTest, ExecSync_Creates_New_File_If_Previous_Removed) {
        //dummy files
        QTemporaryFile f1;
        f1.open();
        f1.write(QByteArray("some text data 1"));
        f1.close();

        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1/file1.txt"));

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f1.fileName()));
        e1->binarySource->open();

        //the object tested
        const QString name = QDir::tempPath() + QDir::separator() + "test_"
            + QUuid::createUuid().toString(QUuid::WithoutBraces);
        ExecStore store(name);

        //extract the 1st file
        const QString sync1 = store.execSync(e1, []() { return false; });

        //remove the file
        QFile::remove(sync1);

        //extract new ont
        const QString sync2 = store.execSync(e1, []() { return false; });

        //check the result
        ASSERT_NE(sync1, sync2);
    }

    TEST(ExecStoreTest, ExecSync_Creates_New_File_If_Previous_Modified) {
        //dummy files
        QTemporaryFile f1;
        f1.open();
        f1.write(QByteArray("some text data 1"));
        f1.close();

        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1/file1.txt"));

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f1.fileName()));
        e1->binarySource->open();

        //the object tested
        const QString name = QDir::tempPath() + QDir::separator() + "test_"
            + QUuid::createUuid().toString(QUuid::WithoutBraces);
        ExecStore store(name);

        //extract the 1st file
        const QString sync1 = store.execSync(e1, []() { return false; });

        //modify the file
        QFile s1(sync1);
        s1.open(QIODeviceBase::Append);
        s1.write(QByteArray("some more content"));
        s1.close();

        //extract new ont
        const QString sync2 = store.execSync(e1, []() { return false; });

        //check the result
        ASSERT_NE(sync1, sync2);
    }

    class Win32File {
    public:
        Win32File(const QString& path):
            file_(INVALID_HANDLE_VALUE),
            path_(path) {
        }

        ~Win32File() {
            close();
        }

        void open() {
            if (file_ == INVALID_HANDLE_VALUE) {
                file_ = CreateFileA(
                    path_.toLocal8Bit().constData(),
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ,
                    nullptr,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    nullptr
                );
                assert(file_ != INVALID_HANDLE_VALUE);
            }
        }

        void close() {
            if (file_ != INVALID_HANDLE_VALUE) {
                CloseHandle(file_);
                file_ = INVALID_HANDLE_VALUE;
            }
        }

    private:
        HANDLE file_;
        QString path_;
    };

    TEST(ExecStoreTest, ExecSync_Creates_New_File_If_Previous_Locked) {
        //dummy files
        QTemporaryFile f1;
        f1.open();
        f1.write(QByteArray("some text data 1"));
        f1.close();

        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1/file1.txt"));

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f1.fileName()));
        e1->binarySource->open();

        //the object tested
        const QString name = QDir::tempPath() + QDir::separator() + "test_"
            + QUuid::createUuid().toString(QUuid::WithoutBraces);
        ExecStore store(name);

        //extract the 1st copy
        const QString sync1 = store.execSync(e1, []() { return false; });

        //lock the copy and extract the 2nd copy
        Win32File s1(sync1);
        s1.open();
        const QString sync2 = store.execSync(e1, []() { return false; });

        //release the lock and extract the 3rd copy
        s1.close();
        const QString sync3 = store.execSync(e1, []() { return false; });

        //check the result
        ASSERT_EQ(sync1, sync3);
        ASSERT_NE(sync1, sync2);
    }
}
