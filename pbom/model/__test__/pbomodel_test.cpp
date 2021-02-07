#include "model/pbomodel.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "QScopedPointer"
#include "io/pboheaderio.h"
#include "model/pboentry.h"
#include "model/pbomodelevents.h"

namespace pboman3::test {
    struct PboModelTest {
        static void writeEntry(QTemporaryFile& t) {
            t.open();

            PboFile p(t.fileName());
            p.open(QIODeviceBase::WriteOnly);
            const PboHeaderIO io(&p);

            const QScopedPointer<PboEntry> e1(new PboEntry("f1", PboPackingMethod::Packed, 0x01010101, 0x02020202,
                                                           0x03030303, 0x04040404));
            const PboEntry e2 = PboEntry::makeBoundary();

            io.writeEntry(e1.get());
            io.writeEntry(&e2);
            p.close();
            t.close();
        }
    };

    TEST(PboModelTest, LoadFile_Loads_File_Without_Headers) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method && assert
        int i = 0;
        auto callback = [&i](const PboModelEvent* e) {
            const auto j = i++;
            if (j == 0) {
                ASSERT_TRUE(dynamic_cast<const PboLoadBeginEvent*>(e));
            } else if (j == 1) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(evt->entry->fileName, "f1");
                ASSERT_EQ(evt->entry->packingMethod, PboPackingMethod::Packed);
                ASSERT_EQ(evt->entry->originalSize, 0x01010101);
                ASSERT_EQ(evt->entry->reserved, 0x02020202);
                ASSERT_EQ(evt->entry->timestamp, 0x03030303);
                ASSERT_EQ(evt->entry->dataSize, 0x04040404);
            } else if (j == 2) {
                ASSERT_TRUE(dynamic_cast<const PboLoadCompleteEvent*>(e));
            }
        };

        PboModel m;
        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());

        //verify the results
        ASSERT_EQ(i, 3);
    }

    TEST(PboModelTest, LoadFile_Loads_File_With_Headers) {
        //build a mock pbo file
        QTemporaryFile t;
        t.open();

        PboFile p(t.fileName());
        p.open(QIODeviceBase::WriteOnly);
        const PboHeaderIO io(&p);

        const PboEntry e0 = PboEntry::makeSignature();
        const PboEntry e1("f1", PboPackingMethod::Packed, 0x01010101, 0x02020202,
                          0x03030303, 0x04040404);
        const PboEntry e2("f2", PboPackingMethod::Uncompressed, 0x05050505, 0x06060606,
                          0x07070707, 0x08080808);
        const PboEntry e3 = PboEntry::makeBoundary();

        const PboHeader h1("p1", "v1");
        const PboHeader h2("p2", "v2");
        const PboHeader h3 = PboHeader::makeBoundary();

        io.writeEntry(&e0);
        io.writeHeader(&h1);
        io.writeHeader(&h2);
        io.writeHeader(&h3);
        io.writeEntry(&e1);
        io.writeEntry(&e2);
        io.writeEntry(&e3);
        p.close();
        t.close();

        //call the method && assert

        int i = 0;
        auto callback = [&i](const PboModelEvent* e) {
            auto j = i++;
            if (j == 0) {
                ASSERT_TRUE(dynamic_cast<const PboLoadBeginEvent*>(e));
            } else if (j == 1) {
                const auto* evt = dynamic_cast<const PboHeaderCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(evt->header->name, "p1");
                ASSERT_EQ(evt->header->value, "v1");
            } else if (j == 2) {
                const auto* evt = dynamic_cast<const PboHeaderCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(evt->header->name, "p2");
                ASSERT_EQ(evt->header->value, "v2");
            } else if (j == 3) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(evt->entry->fileName, "f1");
                ASSERT_EQ(evt->entry->packingMethod, PboPackingMethod::Packed);
                ASSERT_EQ(evt->entry->originalSize, 0x01010101);
                ASSERT_EQ(evt->entry->reserved, 0x02020202);
                ASSERT_EQ(evt->entry->timestamp, 0x03030303);
                ASSERT_EQ(evt->entry->dataSize, 0x04040404);
            } else if (j == 4) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(evt->entry->fileName, "f2");
                ASSERT_EQ(evt->entry->packingMethod, PboPackingMethod::Uncompressed);
                ASSERT_EQ(evt->entry->originalSize, 0x05050505);
                ASSERT_EQ(evt->entry->reserved, 0x06060606);
                ASSERT_EQ(evt->entry->timestamp, 0x07070707);
                ASSERT_EQ(evt->entry->dataSize, 0x08080808);
            } else if (j == 5) {
                ASSERT_TRUE(dynamic_cast<const PboLoadCompleteEvent*>(e));
            }
        };

        PboModel m;
        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());

        //verify the results
        ASSERT_EQ(i, 6);
    }

    TEST(PboModelTest, ScheduleEntryDelete_Emits_Event) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        PboEntry* entry = nullptr;
        auto i = 0;

        auto callback = [&entry, &i](const PboModelEvent* e) {
            const auto j = i++;
            if (j == 1) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                entry = const_cast<PboEntry*>(evt->entry);
            } else if (j == 3) {
                const auto* evt = dynamic_cast<const PboEntryDeleteScheduledEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(entry, evt->entry);
            }
        };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());
        ASSERT_TRUE(entry);
        m.scheduleEntryDelete(entry);
        ASSERT_EQ(i, 4);
    }

    TEST(PboModelTest, ScheduleEntryDelete_Emits_No_Events) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        auto i = 0;
        auto callback = [&i](const PboModelEvent* e) { i++; };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());
        const PboEntry e0("not-existing", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        m.scheduleEntryDelete(&e0);
        ASSERT_EQ(i, 3);
    }

    TEST(PboModelTest, CancelEntryDelete_Emits_Event) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        PboEntry* entry = nullptr;
        auto i = 0;

        auto callback = [&entry, &i](const PboModelEvent* e) {
            const auto j = i++;
            if (j == 1) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                entry = const_cast<PboEntry*>(evt->entry);
            } else if (j == 4) {
                const auto* evt = dynamic_cast<const PboEntryDeleteCanceledEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(entry, evt->entry);
            }
        };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());
        ASSERT_TRUE(entry);
        m.scheduleEntryDelete(entry);
        m.cancelEntryDelete(entry);
        ASSERT_EQ(i, 5);
    }

    TEST(PboModelTest, CancelEntryDelete_Emits_No_Events) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        PboEntry* entry = nullptr;
        auto i = 0;
        auto callback = [&entry, &i](const PboModelEvent* e) {
            const auto j = i++;
            if (j == 1) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                entry = const_cast<PboEntry*>(evt->entry);
            }
        };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());
        ASSERT_TRUE(entry);
        m.cancelEntryDelete(entry); //the entry is not scheduled to delete, so no events should fire
        ASSERT_EQ(i, 3);
    }

    TEST(PboModelTest, ScheduleEntryMove_Emits_Events_If_Move_Original_Entry) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        PboEntry* entry = nullptr;
        auto i = 0;

        auto callback = [&entry, &i](const PboModelEvent* e) {
            const auto j = i++;
            if (j == 1) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_TRUE(evt->entry);
                entry = const_cast<PboEntry*>(evt->entry);
            } else if (j == 3) {
                const auto* evt = dynamic_cast<const PboEntryMoveScheduledEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(entry, evt->entry);

                ASSERT_TRUE(evt->movedEntry);
                ASSERT_NE(evt->movedEntry, entry);
                ASSERT_EQ(evt->movedEntry->fileName, "some-new-path");
                ASSERT_EQ(evt->movedEntry->packingMethod, entry->packingMethod);
                ASSERT_EQ(evt->movedEntry->originalSize, entry->originalSize);
                ASSERT_EQ(evt->movedEntry->reserved, entry->reserved);
                ASSERT_EQ(evt->movedEntry->timestamp, entry->timestamp);
                ASSERT_EQ(evt->movedEntry->dataSize, entry->dataSize);
            }
        };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());
        ASSERT_TRUE(entry);
        m.scheduleEntryMove(entry, "some-new-path");

        ASSERT_EQ(i, 4);
    }

    TEST(PboModelTest, ScheduleEntryMove_Emits_Events_If_Move_Moved_Entry) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        PboEntry* entry = nullptr;
        PboEntry* movedEntry = nullptr;
        auto i = 0;

        auto callback = [&entry, &movedEntry, &i](const PboModelEvent* e) {
            const auto j = i++;
            if (j == 1) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                entry = const_cast<PboEntry*>(evt->entry);
            } else if (j == 3) {
                const auto* evt = dynamic_cast<const PboEntryMoveScheduledEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(entry, evt->entry);

                ASSERT_EQ(evt->movedEntry->fileName, "some-new-path-1");
                movedEntry = const_cast<PboEntry*>(evt->movedEntry);
            } else if (j == 4) {
                const auto* evt = dynamic_cast<const PboEntryMoveCanceledEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(entry, evt->entry);
                ASSERT_EQ(movedEntry, evt->movedEntry);
            } else if (j == 5) {
                const auto* evt = dynamic_cast<const PboEntryMoveScheduledEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(evt->movedEntry->fileName, "some-new-path-2");
            }
        };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());
        ASSERT_TRUE(entry);
        m.scheduleEntryMove(entry, "some-new-path-1");
        m.scheduleEntryMove(entry, "some-new-path-2");

        ASSERT_EQ(i, 6);
    }

    TEST(PboModelTest, ScheduleEntryMove_Emits_No_Events) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        auto i = 0;
        auto callback = [&i](const PboModelEvent* e) { i++; };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());

        const PboEntry e0("not-existing", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        m.scheduleEntryMove(&e0, "some-new-path");
        ASSERT_EQ(i, 3);
    }

    TEST(PboModelTest, CancelEntryMove_Emits_Events) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        PboEntry* entry = nullptr;
        PboEntry* movedEntry = nullptr;
        auto i = 0;

        auto callback = [&entry, &movedEntry, &i](const PboModelEvent* e) {
            const auto j = i++;
            if (j == 1) {
                const auto* evt = dynamic_cast<const PboEntryCreatedEvent*>(e);
                ASSERT_TRUE(evt);
                entry = const_cast<PboEntry*>(evt->entry);
            } else if (j == 3) {
                const auto* evt = dynamic_cast<const PboEntryMoveScheduledEvent*>(e);
                ASSERT_TRUE(evt);
                movedEntry = const_cast<PboEntry*>(evt->movedEntry);
            } else if (j == 4) {
                const auto* evt = dynamic_cast<const PboEntryMoveCanceledEvent*>(e);
                ASSERT_TRUE(evt);
                ASSERT_EQ(evt->movedEntry, movedEntry);
            }
        };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());
        ASSERT_TRUE(entry);
        m.scheduleEntryMove(entry, "some-new-path");
        m.cancelEntryMove(entry);

        ASSERT_EQ(i, 5);
    }

    TEST(PboModelTest, CancelEntryMove_Emits_No_Events) {
        //build a mock pbo file
        QTemporaryFile t;
        PboModelTest::writeEntry(t);

        //call the method and verify
        PboModel m;
        auto i = 0;

        auto callback = [&i](const PboModelEvent* e) {i++; };

        QObject::connect(&m, &PboModel::onEvent, callback);
        m.loadFile(t.fileName());

        const PboEntry e0("not-existing", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        m.cancelEntryMove(&e0);

        ASSERT_EQ(i, 3);
    }
}
