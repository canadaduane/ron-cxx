#include "../mem_store.hpp"
#include "testutil.hpp"

using namespace ron;
using namespace std;

using Frame = TextFrame;
using MemStore = InMemoryStore<TextFrame>;
using Store = MemStore;
using Iterator = typename Store::Iterator;

TEST(MemStore, Ends) {
    TmpDir tmp;
    tmp.cd("Ends");
    Store store;
    auto now = Uuid::Now();
    Uuid id{now, Word::random()};
    ASSERT_TRUE(IsOK(store.Open(id)));
    Frame frame;
    ASSERT_TRUE(IsOK(store.Read(Key{}, frame)));
    ASSERT_FALSE(frame.empty());
    ASSERT_TRUE(IsOK(store.Read(END_KEY, frame)));
    ASSERT_TRUE(frame.empty());
    ASSERT_FALSE(IsOK(store.Read(Key{id,LWW_RDT_FORM}, frame)));
    Iterator i{store};
    ASSERT_EQ(i.key(), END_KEY);
    ASSERT_FALSE(i.value().valid());
    
    ASSERT_TRUE(IsOK(i.SeekTo(END_KEY, false)));
    ASSERT_EQ(i.key(), END_KEY);
    ASSERT_FALSE(i.value().valid());
    ASSERT_TRUE(IsOK(i.SeekTo(END_KEY, true)));
    ASSERT_EQ(i.key(), END_KEY);
    ASSERT_FALSE(i.value().valid());
    
    ASSERT_TRUE(IsOK(i.SeekTo(Key{}, true)));
    ASSERT_EQ(i.key(), Key{});
    ASSERT_TRUE(i.value().valid());
    ASSERT_TRUE(IsOK(i.SeekTo(Key{}, false)));
    ASSERT_EQ(i.key(), Key{});
    ASSERT_TRUE(i.value().valid());
    
    // empty store, 2 records: zero and end
    ASSERT_TRUE(IsOK(i.Next()));
    ASSERT_TRUE(IsOK(i.SeekTo(END_KEY, true)));
    ASSERT_EQ(i.key(), END_KEY);
    ASSERT_FALSE(i.value().valid());
    
    // Next() at the end => end, returns ENDOFINPUT
    ASSERT_FALSE(i.Next()==Status::ENDOFINPUT);
    ASSERT_TRUE(IsOK(i.SeekTo(END_KEY, true)));
    ASSERT_EQ(i.key(), END_KEY);
    ASSERT_FALSE(i.value().valid());
}



TEST(MemStore, Range) {
    String A{"@1+A :lww 'int' 1;"};
    String B{"@2+A :1+A 'string' 'str';"};
    String M{"@1+A :lww 'int' 1, @2+A 'string' 'str';"};
    MemStore store;
    Frame a{A}, b{B}, m{M};
    Key key{Uuid{"1+A"}, LWW_FORM_UUID};
    assert(store.Write(key, a));
    assert(store.Write(key, b));
    Frame _m;
    assert(store.Read(key, _m));
    assert(CompareFrames(m, _m));
    assert(store.Read(key, _m));
    assert(CompareFrames(m, _m));
}

TEST (MemStore, Iterator) {
    String A{"@1+A :lww 'int' 1;"};
    String B{"@2+A :1+A 'string' 'str';"};
    String M{"@1+A :lww 'int' 1, @2+A 'string' 'str';"};
    MemStore store;
    Frame a{A}, b{B}, m{M};
    Key key{Uuid{"1+A"}, LWW_FORM_UUID};
    assert(store.Write(key, a));
    assert(store.Write(key, b));
    MemStore::Iterator i{store};
    i.SeekTo(Key{});
    auto c = i.value();
    i.Next();
    assert(i.key()==END_KEY);
    auto mc = m.cursor();
    assert(CompareWithCursors(mc, c));
    //auto j = store.begin();
    //assert(CompareFrames(m, (*j).second));
}

int main (int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}
