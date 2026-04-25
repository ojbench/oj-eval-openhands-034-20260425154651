#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>

// Skip list implementation supporting generic comparable type T
// Only relies on operator< for ordering

template <typename T>
class SkipList {
private:
    struct BaseNode {
        std::vector<BaseNode*> next;
        explicit BaseNode(int level) : next(level, nullptr) {}
        virtual ~BaseNode() = default;
    };
    struct DataNode : BaseNode {
        T value;
        DataNode(int level, const T& val) : BaseNode(level), value(val) {}
    };

    // Maximum allowed levels (enough for up to ~1e9 elements with p=0.5)
    static constexpr int MAX_LEVEL = 32;

    BaseNode* head;
    int level; // current highest level count of head (1..MAX_LEVEL)

    static bool lessThan(const T& a, const T& b) { return a < b; }
    static bool equalTo(const T& a, const T& b) { return !lessThan(a, b) && !lessThan(b, a); }

    int randomLevel() {
        int lvl = 1;
        while (lvl < MAX_LEVEL && (std::rand() & 1)) ++lvl;
        return lvl;
    }

public:
    SkipList() : head(new BaseNode(MAX_LEVEL)), level(1) {
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned>(std::time(nullptr)) ^ 0x9e3779b9U);
            seeded = true;
        }
    }

    ~SkipList() {
        BaseNode* cur = head->next[0];
        while (cur) {
            BaseNode* nxt = cur->next[0];
            delete cur;
            cur = nxt;
        }
        delete head;
    }

    void insert(const T& item) {
        std::vector<BaseNode*> update(MAX_LEVEL, nullptr);
        BaseNode* cur = head;
        for (int i = level - 1; i >= 0; --i) {
            while (cur->next[i] && lessThan(static_cast<DataNode*>(cur->next[i])->value, item)) cur = cur->next[i];
            update[i] = cur;
        }
        BaseNode* nxt = cur->next[0];
        if (nxt && equalTo(static_cast<DataNode*>(nxt)->value, item)) return;

        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level; i < newLevel; ++i) update[i] = head;
            level = newLevel;
        }
        DataNode* node = new DataNode(newLevel, item);
        for (int i = 0; i < newLevel; ++i) {
            node->next[i] = update[i]->next[i];
            update[i]->next[i] = node;
        }
    }

    bool search(const T& item) {
        BaseNode* cur = head;
        for (int i = level - 1; i >= 0; --i) {
            while (cur->next[i] && lessThan(static_cast<DataNode*>(cur->next[i])->value, item)) cur = cur->next[i];
        }
        cur = cur->next[0];
        return cur && equalTo(static_cast<DataNode*>(cur)->value, item);
    }

    void deleteItem(const T& item) {
        std::vector<BaseNode*> update(MAX_LEVEL, nullptr);
        BaseNode* cur = head;
        for (int i = level - 1; i >= 0; --i) {
            while (cur->next[i] && lessThan(static_cast<DataNode*>(cur->next[i])->value, item)) cur = cur->next[i];
            update[i] = cur;
        }
        cur = cur->next[0];
        if (!cur || !equalTo(static_cast<DataNode*>(cur)->value, item)) return;

        for (int i = 0; i < level; ++i) {
            if (update[i]->next[i] != cur) break;
            update[i]->next[i] = cur->next[i];
        }
        delete cur;
        while (level > 1 && head->next[level - 1] == nullptr) --level;
    }
};

#endif
