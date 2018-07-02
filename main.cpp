#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <istream>
#include <ostream>
#include <ctime>


using std::vector;
using std::cout;
using std::cin;
using std::swap;
using std::pair;
using std::endl;


class CartesianTree {
private:
    static std::uniform_int_distribution<long long> id_;
    static std::default_random_engine eng_;
    struct Node {
        long long priority_;
        long long value_;
        long long size_;
        long long sum_;
        bool promise_assign_;
        long long assign_;
        long long add_;
        bool reversed_;
        Node* left_;
        Node* right_;
        long long i_suf_;
        long long d_suf_;
        long long i_pref_;
        long long d_pref_;
        long long l_el_;
        long long r_el_;

        explicit Node(long long value_) : value_(value_), size_(1), sum_(value_), promise_assign_(false),
                                          assign_(0), add_(0), right_(NULL), left_(NULL), reversed_(false), l_el_(value_), r_el_(value_),
                                          i_suf_(1), i_pref_(1), d_suf_(1), d_pref_(1) {
            priority_ = id_(eng_);
        }
    };

    Node* root_;

    static long long Size(const Node* root_) {
        if (root_ == NULL)
            return 0;
        return root_->size_;
    }

    static void Push(Node* root_) {
        if (root_ == NULL)
            return;

        if (root_->promise_assign_) {
            if (root_->left_ != NULL) {
                PushChild(root_->left_, root_);
            }
            if (root_->right_ != NULL) {
                PushChild(root_->right_, root_);
            }
            root_->promise_assign_ = false;
            root_->value_ += root_->add_;
            root_->l_el_ += root_->add_;
            root_->r_el_ += root_->add_;
            root_->sum_ += root_->size_ * root_->add_;
            root_->add_ = 0;
            root_->d_suf_ = Size(root_);
            root_->d_pref_ = Size(root_);
            root_->i_pref_ = Size(root_);
            root_->d_pref_ = Size(root_);
        } else {
            root_->sum_ = root_->sum_ + root_->add_ * root_->size_;
            root_->value_ += root_->add_;
            root_->l_el_ += root_->add_;
            root_->r_el_ += root_->add_;
            if(root_->left_ != NULL)
                root_->left_->add_ += root_->add_;
            if(root_->right_ != NULL)
                root_->right_->add_ += root_->add_;
            root_->add_ = 0;
        }

        PushReversed(root_);
    }

    static void PushChild(Node* child, Node* root_) {
        child->promise_assign_ = true;
        child->assign_ = root_->assign_ + root_->add_;
        child->value_ = root_->assign_ + root_->add_;
        child->r_el_ = root_->assign_ + root_->add_;
        child->l_el_ = root_->assign_ + root_->add_;
        child->sum_ = (root_->assign_ + root_->add_) * Size(child);
        child->i_suf_ = Size(child);
        child->i_pref_ = Size(child);
        child->d_suf_ = Size(child);
        child->d_pref_ = Size(child);
        child->add_ = 0;
    }

    static void PushReversed(Node* root_) {
        if (root_->reversed_) {
            if (root_->left_ != NULL){
                root_->left_->reversed_ ^= true;
            }
            if (root_->right_ != NULL) {
                root_->right_->reversed_ ^= true;
            }
            root_->reversed_ = false;
            swap(root_->left_, root_->right_);
            swap(root_->i_suf_, root_->i_pref_);
            swap(root_->d_suf_, root_->d_pref_);
            swap(root_->r_el_, root_->l_el_);
        }
    }

    static void Relax(Node*& root_) {
        if (root_ == NULL)
            return;

        Push(root_->left_);
        Push(root_->right_);

        RelaxSumSizeEdges(root_);

        PreRelaxPreSufix(root_);

        UniversalRelax(root_->i_suf_, root_->right_, root_->left_, root_->value_, (root_->right_ ? root_->right_->l_el_ : 0), (root_->left_ ? root_->left_->r_el_ : 0), root_->value_, (root_->left_ ? root_->left_->i_suf_ : 0));
        UniversalRelax(root_->d_suf_, root_->right_, root_->left_, (root_->right_ ? root_->right_->l_el_ : 0), root_->value_, root_->value_, (root_->left_ ? root_->left_->r_el_ : 0), (root_->left_ ? root_->left_->d_suf_ : 0));
        UniversalRelax(root_->i_pref_, root_->left_, root_->right_, root_->value_, (root_->left_ ? root_->left_->r_el_ : 0), (root_->right_ ? root_->right_->l_el_ : 0), root_->value_, (root_->right_ ? root_->right_->i_pref_ : 0));
        UniversalRelax(root_->d_pref_, root_->left_, root_->right_, (root_->left_ ? root_->left_->r_el_ : 0), root_->value_, root_->value_, (root_->right_ ? root_->right_->l_el_ : 0), (root_->right_ ? root_->right_->d_pref_ : 0));

    }

    static void UniversalRelax(long long& fix, Node*& first, Node*& second, long long bigger1, long long lower1, long long bigger2, long long lower2, long long add) {
        if (fix == Size(first)) {
            if (first == NULL || bigger1 >= lower1) {
                fix++;
                if (second) {
                    if (bigger2 >= lower2) {
                        fix += add;
                    }
                }
            }
        }
    }

    static void RelaxSumSizeEdges(Node*& root_) {
        if (root_ == NULL)
            return;
        root_->size_ = Size(root_->left_) + Size(root_->right_) + 1;

        root_->r_el_ = root_->value_;
        root_->l_el_ = root_->value_;

        root_->sum_ = root_->value_;

        if (root_->right_ != NULL) {
            root_->r_el_ = root_->right_->r_el_;
            root_->sum_ += root_->right_->sum_ + Size(root_->right_) * root_->right_->add_;
        }
        if (root_->left_ != NULL) {
            root_->l_el_ = root_->left_->l_el_;
            root_->sum_ += root_->left_->sum_ + root_->left_->add_ * Size(root_->left_);
        }
    }

    static void PreRelaxPreSufix(Node*& root_) {
        if( root_ == NULL)
            return;
        root_->i_suf_ = 0;
        root_->d_suf_ = 0;
        root_->i_pref_ = 0;
        root_->d_pref_ = 0;
        root_->i_suf_ = GetRight(root_)->i_suf_;
        root_->d_suf_ = GetRight(root_)->d_suf_;
        root_->i_pref_ = GetLeft(root_)->i_pref_;
        root_->d_pref_ = GetLeft(root_)->d_pref_;
    }

    static Node* GetLeft(Node*& root_) {
        if (root_->left_ == NULL)
            return root_;
        return root_->left_;
    }

    static Node* GetRight(Node*& root_) {
        if (root_->right_ == NULL)
            return root_;
        return root_->right_;
    }

    pair<Node*, Node*> ValueSplit(Node* root_, long long val) {
        if (root_ == NULL) {
            return {NULL, NULL};
        }
        Push(root_);

        if (root_->value_ + root_->add_ <= val) {
            pair<Node*, Node*> temp = ValueSplit(root_->right_, val);
            root_->right_ = temp.first;
            Relax(root_);
            return {root_, temp.second};
        } else {
            pair<Node*, Node*> temp = ValueSplit(root_->left_, val);
            root_->left_ = temp.second;
            Relax(root_);
            return {temp.first, root_};
        }
    }

    pair<Node*, Node*> Split(long long key, Node* root_) {
        if (root_ == NULL) {
            return {NULL, NULL};
        }
        Push(root_);

        if (Size(root_->left_) >= key) {
            pair<Node*,Node*> t = Split(key, root_->left_);
            root_->left_ = t.second;
            Relax(root_);
            return {t.first, root_};
        } else {
            pair<Node*, Node*> t = Split(key - Size(root_->left_) - 1, root_->right_);
            root_->right_ = t.first;
            Relax(root_);
            return {root_, t.second};
        }
    }

    Node* Merge(Node* l, Node* r) {
        Push(l);
        Push(r);
        if (l == NULL || r == NULL)
            return l ? l : r;
        if (l->priority_ >= r->priority_) {
            l->right_ = Merge(l->right_, r);
            Relax(l);
            return l;
        } else {
            r->left_ = Merge(l, r->left_);
            Relax(r);
            return r;
        }
    }

    template<typename Func>
    void MakeOperation(Node*& root_, size_t l, size_t r, Func func) {
        if (root_ == NULL) {
            return;
        }
        pair<Node*, Node*> t1 = Split(l - 1, root_);
        pair<Node*, Node*> t2 = Split(r - l + 1, t1.second);
        func(t2.first);
        root_ = Merge(t1.first, Merge(t2.first, t2.second));
    }

public:

    void Reverse(Node*& root_, long long l, long long r) {
        MakeOperation(root_, l, r, [](Node* root__) {
            root__->reversed_ ^= true;
        });
    }

    long long Write(long long  key) {
        long long ans;
        MakeOperation(root_, key, key, [&ans](Node* root__) {
            ans = root__->value_;
        });
        return ans;
    }

    void Insert(long long key, long long value__) {
        Node* newNode = new Node(value__);
        if(root_ == NULL) {
            root_ = newNode;
            return;
        }
        pair<Node*, Node*> t = Split(key - 1, root_);
        Node* temp = Merge(t.first, newNode);
        root_ = Merge(temp, t.second);
    }

    void Assign(long long l, long long r, long long val) {
        MakeOperation(root_, l, r, [val, l, r](Node* root__) {
            root__->value_ = val;
            root__->l_el_ = val;
            root__->r_el_ = val;
            root__->promise_assign_ = true;
            root__->assign_ = val;
            root__->sum_ = (r - l + 1) * val;
            root__->d_pref_ = root__->size_;
            root__->i_pref_ = root__->size_;
            root__->i_suf_ = root__->size_;
            root__->add_ = 0;
        });
    }

    void Add(long long l, long long r, long long val) {
        MakeOperation(root_, l, r, [val](Node* root__) {
            root__->add_ += val;
        });
    }

    void Delete(long long key) {
        MakeOperation(root_, key, key, [](Node*& root__) {
            delete root__;
            root__ = nullptr;
        });
    }

    long long Sum(long long l, long long r) {
        long long ans;
        MakeOperation(root_, l, r, [&ans](Node* root__) {
            ans = root__->sum_;
        });
        return ans;
    }

    enum {
        I_SUF,
        D_SUF
    };

    bool CheckPermutation(Node*& root_, int flag) {
        int value;
        if (flag) {
            value = root_->d_suf_;
        } else {
            value = root_->i_suf_;
        }
        if (value == Size(root_)) {
            Reverse(root_, 1, Size(root_));
            return true;
        }
        return false;
    }

    pair<pair<Node*, Node*>, pair<Node*, Node*>> PreparePermutation(Node*& root_, int flag) {
        int value;
        if (flag) {
            value = root_->d_suf_;
        } else {
            value = root_->i_suf_;
        }
        pair<Node*, Node*> t1 = Split(Size(root_) - value - 1, root_);
        pair<Node*, Node*> t2 = Split(1, t1.second);
        return {t1, t2};
    };

    void NextPermutation(long long l, long long r) {
        MakeOperation(root_, l, r, [this](Node*& root__) {
            if (CheckPermutation(root__, I_SUF))
                return;
            pair<pair<Node*, Node*>, pair<Node*, Node*>> nodes = PreparePermutation(root__, I_SUF);

            Reverse(nodes.second.second, 1, Size(nodes.second.second));

            pair<Node*, Node*> t3 = ValueSplit(nodes.second.second, nodes.second.first->value_);
            pair<Node*, Node*> t4 = Split(1, t3.second);

            root__ = MergeAll(nodes.first.first,t4.first, t3.first, nodes.second.first, t4.second);
        });
    }

    void PrevPermutation(long long l, long long r) {
        MakeOperation(root_, l, r, [this](Node*& root__) {
            if (CheckPermutation(root__, D_SUF))
                return;
            pair<pair<Node*, Node*>, pair<Node*, Node*>> nodes = PreparePermutation(root__, D_SUF);

            pair<Node *, Node *> t3 = ValueSplit(nodes.second.second, nodes.second.first->value_ - 1);
            pair<Node *, Node *> t4 = Split(Size(t3.first) - 1, t3.first);

            Reverse(t4.first, 1, Size(t4.first));
            Reverse(nodes.second.first, 1, Size(nodes.second.first));
            Reverse(t3.second, 1, Size(t3.second));

            root__ = MergeAll(nodes.first.first, t4.second, t3.second, nodes.second.first, t4.first);
        });
    }

    Node* MergeAll(Node*& root_1, Node*& root_2, Node*& root_3, Node*& root_4, Node*& root_5) {
        root_1 = Merge(root_1, root_2);
        root_1 = Merge(root_1, root_3);
        root_1 = Merge(root_1, root_4);
        root_1 = Merge(root_1, root_5);
        return root_1;
    }

    size_t GetSize() const {
        return root_->size_;
    }

    CartesianTree() : root_(NULL) {}

    ~CartesianTree() {
        if (root_->left_ != NULL)
            delete root_->left_;
        if (root_->right_ != NULL)
            delete root_->right_;
        delete root_;
    }
};

std::uniform_int_distribution<long long> CartesianTree::id_ = std::uniform_int_distribution<long long>(1, std::numeric_limits<long long>::max());
std::default_random_engine CartesianTree::eng_;

/*-------------------------------------------------*/

enum {
    SUM = 1,
    INSERT = 2,
    DELETE = 3,
    ASSIGN = 4,
    ADD = 5,
    NEXTPERMUTATION = 6,
    PREVPERMUTATION = 7
};

class Query {
public:
    virtual void Process(CartesianTree& tree) = 0;
    virtual ~Query() {};
};

class Sum : public Query {
    long long l, r, ans;
public:
    explicit Sum(long long l, long long r) : l(l), r(r), ans(0) {};

    void Process(CartesianTree& tree) override {
        ans = tree.Sum(l, r);
    }

    long long sum() const {
        return ans;
    };

    ~Sum() = default;
};

class Insert : public Query {
    long long x, pos;
public:
    explicit Insert(long long pos, long long x) : pos(pos), x(x) {}

    void Process(CartesianTree& tree) override {
        tree.Insert(pos, x);
    }


    ~Insert() = default;
};

class Delete : public Query {
    long long pos;
public:
    explicit Delete(long long pos) : pos(pos) {}

    void Process(CartesianTree& tree) override {
        tree.Delete(pos);
    }

    ~Delete() = default;
};

class Assign : public Query {
    long long l, r, x;
public:
    explicit Assign(long long l, long long r, long long x) : l(l), r(r), x(x) {}

    void Process(CartesianTree& tree) override {
        tree.Assign(l, r, x);
    }

    ~Assign() = default;
};

class Add : public Query {
    long long l, r, x;
public:
    explicit Add(long long l, long long r, long long x) : l(l), r(r), x(x) {}

    void Process(CartesianTree& tree) override {
        tree.Add(l, r, x);
    }
    ~Add() = default;
};

class NextPermutation : public Query {
    long long l, r;
public:
    explicit NextPermutation(long long l, long long r) : l(l), r(r) {}

    void Process(CartesianTree& tree) override {
        tree.NextPermutation(l, r);
    }

    ~NextPermutation() = default;
};

class PrevPermutation : public Query {
    long long l, r;
public:
    explicit PrevPermutation(long long l, long long r) : l(l), r(r) {}

    void Process(CartesianTree& tree) override {
        tree.PrevPermutation(l, r);
    }
    ~PrevPermutation() = default;
};

Query* GetQuery(std::istream& is, int num) {
    switch (num) {
        case SUM: {
            long long l, r;
            is >> l >> r;
            ++l;
            ++r;
            Sum* query = new Sum(l, r);
            return query;
            break;
        }
        case INSERT: {
            long long x, pos;
            is >> x >> pos;
            ++pos;
            Insert* query = new Insert(pos, x);
            return query;
            break;
        }
        case DELETE: {
            long long pos;
            is >> pos;
            ++pos;
            Delete* query = new Delete(pos);
            return query;
            break;
        }
        case ASSIGN: {
            long long x, l, r;
            is >> x >> l >> r;
            ++l;
            ++r;
            Assign* query = new Assign(l, r, x);
            return query;
            break;
        }
        case NEXTPERMUTATION: {
            long long l, r;
            is >> l >> r;
            ++l;
            ++r;
            NextPermutation* query = new NextPermutation(l, r);
            return query;
            break;
        }
        case PREVPERMUTATION: {
            long long l, r;
            is >> l >> r;
            ++l;
            ++r;
            PrevPermutation* query = new PrevPermutation(l, r);
            return query;
            break;
        }
        default: {
            long long x, l, r;
            is >> x >> l >> r;
            ++l;
            ++r;
            Add* query = new Add(l, r, x);
            return query;
            break;
        }
    }
}

//void QueryDestruct(std::vector<Query*>& v) {
//    for (int i = 0; i < v.size(); ++i) {
//        delete v[i];
//    }
//}

std::vector<Query*> GetValues(std::istream& is) {
    std::vector<Query*> input;
    size_t n;
    is >> n;
    for (size_t i = 1; i <= n; ++i) {
        long long x;
        is >> x;
        input.push_back(static_cast<Query*>(new Insert(i, x)));
    }
    return input;
}

struct Input {
    std::vector<Query*> input;
//    ~Input() {
//        for (int i = 0; i < input.size(); ++i) {
//            delete input[i];
//        }
//    }
    Input() = default;

    Input(Input&& other)
            : input(std::move(other.input))
    {}

    Input& operator = (Input&& other) {
        input = std::move(other.input);
        return *this;
    }

    Input(const Input& other) = delete;
    Input operator=(const Input& other) = delete;
};

Input GetInput(std::istream& is) {
    Input current_input;
    current_input.input = std::move(GetValues(is));
    size_t m;
    is >> m;
    for (size_t i = 0; i < m; ++i) {
        int operation;
        is >> operation;
        current_input.input.push_back(GetQuery(is, operation));
    }

    return current_input;
}

struct Output {
    std::vector<long long> output_sum_;
    std::vector<long long> output_values;
};

class ProcessManager {
    CartesianTree tree;
    Output output;
    Input input;
public:
    ProcessManager(Input&& input_) {
        input = std::move(input_);
    }

    const Output& Run_() {
        WriteSums(input.input, output.output_sum_);
        WriteValues(output.output_values);
        return output;
    }

    void WriteSums(const std::vector<Query*>& input_, std::vector<long long>& output_sums) {
        for (size_t i = 0; i < input_.size(); ++i) {
            input_[i]->Process(tree);
            Sum* checker = dynamic_cast<Sum*>(input_[i]);
            if (checker != nullptr) {
                output.output_sum_.push_back(checker->sum());
            }
        }
    }

    void WriteValues(std::vector<long long>& output_values) {
        for (size_t i = 1; i <= tree.GetSize(); ++i) {
            output_values.push_back(tree.Write(i));
        }
    }

    ~ProcessManager() {
        for (int i = 0; i < input.input.size(); ++i) {
            delete input.input[i];
        }
    }
};

void WriteOutput(const std::vector<long long>& output, std::ostream& os, char c) {
    for (const auto& i : output) {
        os << i << c;
    }
}

void GetOutput(const Output& output_, std::ostream& os) {
    WriteOutput(output_.output_sum_, os, '\n');
    WriteOutput(output_.output_values, os, ' ');
}

Output Run(Input&& input) {
    ProcessManager manager(std::move(input));
    return manager.Run_();
}
void Solve() {
    Input input = GetInput(std::cin);
    Output output = Run(std::move(input));
    GetOutput(output, std::cout);
}

int main() {
    Solve();
    return 0;
}
