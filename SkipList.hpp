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
    struct Node {
        T value;
        std::vector<Node*> next;
        Node(int level, const T& val) : value(val), next(level, nullptr) {}
        // Head/sentinel constructor: value is unused
        explicit Node(int level) : value(), next(level, nullptr) {}
    };

    // Maximum allowed levels (enough for up to ~1e9 elements with p=0.5)
    static constexpr int MAX_LEVEL = 32;
    // Promotion probability = 0.5 via coin flip on LSB of rand()

    Node* head;
    int level; // current highest level count of head (1..MAX_LEVEL)

    static bool lessThan(const T& a, const T& b) {
        return a < b;
    }
    static bool equalTo(const T& a, const T& b) {
        return !lessThan(a, b) && !lessThan(b, a);
    }

    int randomLevel() {
        int lvl = 1;
        // Use bit property for speed; on average p=0.5
        while (lvl < MAX_LEVEL && (std::rand() & 1)) ++lvl;
        return lvl;
    }

public:
    SkipList() : head(new Node(MAX_LEVEL)), level(1) {
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned>(std::time(nullptr)) ^ 0x9e3779b9U);
            seeded = true;
        }
    }

    ~SkipList() {
        Node* cur = head->next[0];
        while (cur) {
            Node* nxt = cur->next[0];
            delete cur;
            cur = nxt;
        }
        delete head;
    }

    void insert(const T& item) {
        std::vector<Node*> update(MAX_LEVEL, nullptr);
        Node* cur = head;
        for (int i = level - 1; i >= 0; --i) {
            while (cur->next[i] && lessThan(cur->next[i]->value, item)) cur = cur->next[i];
            update[i] = cur;
        }
        Node* nxt = cur->next[0];
        if (nxt && equalTo(nxt->value, item)) return; // already exists

        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level; i < newLevel; ++i) update[i] = head;
            level = newLevel;
        }
        Node* node = new Node(newLevel, item);
        for (int i = 0; i < newLevel; ++i) {
            node->next[i] = update[i]->next[i];
            update[i]->next[i] = node;
        }
    }

    bool search(const T& item) {
        Node* cur = head;
        for (int i = level - 1; i >= 0; --i) {
            while (cur->next[i] && lessThan(cur->next[i]->value, item)) cur = cur->next[i];
        }
        cur = cur->next[0];
        return cur && equalTo(cur->value, item);
    }

    void deleteItem(const T& item) {
        std::vector<Node*> update(MAX_LEVEL, nullptr);
        Node* cur = head;
        for (int i = level - 1; i >= 0; --i) {
            while (cur->next[i] && lessThan(cur->next[i]->value, item)) cur = cur->next[i];
            update[i] = cur;
        }
        cur = cur->next[0];
        if (!cur || !equalTo(cur->value, item)) return; // not found

        for (int i = 0; i < level; ++i) {
            if (update[i]->next[i] != cur) break;
            update[i]->next[i] = cur->next[i];
        }
        delete cur;
        while (level > 1 && head->next[level - 1] == nullptr) --level;
    }
};

#endif
