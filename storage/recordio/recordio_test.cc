// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#include "toft/storage/recordio/recordio.h"

#include <string>

#include "thirdparty/gtest/gtest.h"
#include "toft/base/scoped_ptr.h"
#include "toft/base/string/string_piece.h"
#include "toft/storage/file/file.h"

#include "toft/storage/recordio/document.pb.h"

namespace toft {

const std::string local_file_path = "./test.dat";

class RecordIOTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        r1.set_docid(10);
        r1.mutable_links()->add_forward(20);
        r1.mutable_links()->add_forward(40);
        r1.mutable_links()->add_forward(60);
        r1.add_name();
        r1.mutable_name(0)->add_language();
        r1.mutable_name(0)->mutable_language(0)->set_code("en-us");
        r1.mutable_name(0)->mutable_language(0)->set_country("us");
        r1.mutable_name(0)->add_language();
        r1.mutable_name(0)->mutable_language(1)->set_code("en");
        r1.mutable_name(0)->set_url("http://A");
        r1.add_name();
        r1.mutable_name(1)->set_url("http://B");
        r1.add_name();
        r1.mutable_name(2)->add_language();
        r1.mutable_name(2)->mutable_language(0)->set_code("en-gb");
        r1.mutable_name(2)->mutable_language(0)->set_country("gb");

        r2.set_docid(20);
        r2.mutable_links()->add_backward(10);
        r2.mutable_links()->add_backward(30);
        r2.mutable_links()->add_forward(80);
        r2.add_name();
        r2.mutable_name(0)->set_url("http://C");

        r3.set_docid(30);
        r3.mutable_links()->add_backward(100);
        r3.add_name();
        r3.mutable_name(0)->set_url("http://D");
    }
    virtual void TearDown() {
        m_reader.reset();
        m_writer.reset();
    }

protected:
    scoped_ptr<RecordReader> m_reader;
    scoped_ptr<RecordWriter> m_writer;
    recordio_test::Document r1;
    recordio_test::Document r2;
    recordio_test::Document r3;
};

TEST_F(RecordIOTest, TestNextMessageSequnce) {
    // write record
    scoped_ptr<File> file(File::Open("./test.dat", "a"));
    m_writer.reset(new RecordWriter(file.get()));

    std::string r1_str = r1.SerializeAsString();
    std::string r2_str = r2.SerializeAsString();
    std::string r3_str = r3.SerializeAsString();

    ASSERT_TRUE(m_writer->WriteMessage(r1));
    ASSERT_TRUE(m_writer->WriteMessage(r2));
    ASSERT_TRUE(m_writer->WriteMessage(r3));

    ASSERT_TRUE(file->Close());

    // read record
    file.reset(File::Open("./test.dat", "r"));
    m_reader.reset(new RecordReader(file.get()));

    recordio_test::Document message;
    std::string content;

    ASSERT_TRUE(m_reader->ReadNextMessage(&message));
    ASSERT_TRUE(message.SerializeToString(&content));
    ASSERT_EQ(r1_str, content);

    message.Clear();
    ASSERT_TRUE(m_reader->ReadNextMessage(&message));
    ASSERT_TRUE(message.SerializeToString(&content));
    ASSERT_EQ(r2_str, content);

    message.Clear();
    ASSERT_TRUE(m_reader->ReadNextMessage(&message));
    ASSERT_TRUE(message.SerializeToString(&content));
    ASSERT_EQ(r3_str, content);

    ASSERT_TRUE(file->Close());
}

TEST_F(RecordIOTest, StringPiece) {
    // write record
    scoped_ptr<File> file(File::Open("./test_stringpiece.dat", "a"));
    m_writer.reset(new RecordWriter(file.get()));

    StringPiece sp1("string_piece_1");
    StringPiece sp2("string_piece_2");
    ASSERT_TRUE(m_writer->WriteRecord(sp1));
    ASSERT_TRUE(m_writer->WriteRecord(sp2));
    ASSERT_TRUE(file->Close());

    // read record
    file.reset(File::Open("./test_stringpiece.dat", "r"));
    m_reader.reset(new RecordReader(file.get()));

    StringPiece sp11;
    StringPiece sp22;
    ASSERT_EQ(1, m_reader->Next());
    ASSERT_TRUE(m_reader->ReadRecord(&sp11));
    ASSERT_EQ(sp1, sp11);
    ASSERT_EQ(1, m_reader->Next());
    ASSERT_TRUE(m_reader->ReadRecord(&sp22));
    ASSERT_EQ(sp2, sp22);

    ASSERT_TRUE(file->Close());
}

} // namespace toft
