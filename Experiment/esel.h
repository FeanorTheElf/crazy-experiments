// ================================================================
//       EXPERIMENTAL STRUCTURE EXPOSING LAMBDAS (ESEL)
// ================================================================

#pragma once
#include <type_traits>

template<typename... T>
struct TList {};

template<typename Head, typename... Tail>
struct TList<Head, Tail...> {
    typedef Head head;
    typedef TList<Tail...> tail;
};

template<>
struct TList<> {
    typedef TList<> tail;
};

template<typename T>
constexpr bool IsESEL = false;

template<int id>
struct Var {

    template<typename VarOrderList, typename A, typename... Args>
    auto eval(A fst, Args... tail) -> decltype(auto) {
        if constexpr (std::is_same_v<VarOrderList::head, Var<id>>) {
            return fst;
        }
        else {
            return eval < VarOrderList::tail, Args... >(tail...);
        }
    }
};

const Var<0> x;
const Var<1> y;
const Var<2> z;

template<int id>
constexpr bool IsESEL<Var<id>> = true;

enum struct ESELOp {
    Sum, Prod, Equal, Unequal
};

template<ESELOp op, typename Lhs, typename Rhs>
struct ESELBinExpr {
    Lhs lhs;
    Rhs rhs;

    template<typename VarOrderList, typename... Args>
    auto eval(Args... tail) -> decltype(auto) {
        if constexpr (op == ESELOp::Sum)
            return lhs.eval<VarOrderList, Args...>(tail...) + rhs.eval<VarOrderList, Args...>(tail...);
        else if constexpr (op == ESELOp::Prod)
            return lhs.eval<VarOrderList, Args...>(tail...) * rhs.eval<VarOrderList, Args...>(tail...);
        else if constexpr (op == ESELOp::Equal)
            return lhs.eval<VarOrderList, Args...>(tail...) == rhs.eval<VarOrderList, Args...>(tail...);
        else if constexpr (op == ESELOp::Unequal)
            return lhs.eval<VarOrderList, Args...>(tail...) != rhs.eval<VarOrderList, Args...>(tail...);
    }
};

struct Empty {};

template<typename Cond, typename Lhs, typename Rhs>
struct ESELTernaryOp {
    Cond cond;
    Lhs lhs;
    Rhs rhs;

    template<typename VarOrderList, typename... Args>
    auto eval(Args... tail) -> decltype(auto) {
        if (cond.eval<VarOrderList, Args...>(tail...)) {
            return lhs.eval<VarOrderList, Args...>(tail...);
        }
        else {
            return rhs.eval<VarOrderList, Args...>(tail...);
        }
    }
};

template<typename Cond = Empty, typename Lhs = Empty>
struct ESELTernaryOpBuilder {
    Cond cond;
    Lhs lhs;

    template<typename Next>
    std::enable_if_t < !std::is_same_v<Lhs, Empty>, ESELTernaryOp<Cond, Lhs, Next> > compose(const Next& other) const {
        return ESELTernaryOp<Cond, Lhs, Next>{ cond, lhs, other };
    }

    template<typename Next>
    std::enable_if_t < !std::is_same_v<Cond, Empty> && std::is_same_v<Lhs, Empty>, ESELTernaryOpBuilder<Cond, Next> > compose(const Next& other) const {
        return ESELTernaryOpBuilder<Cond, Next>{ cond, other };
    }

    template<typename Next>
    std::enable_if_t < std::is_same_v<Cond, Empty>, ESELTernaryOpBuilder<Next, Empty> > compose(const Next& other) const {
        return ESELTernaryOpBuilder<Next, Empty>{ other, Empty{} };
    }
};

constexpr ESELTernaryOpBuilder<> choose;

template<ESELOp op, typename Lhs, typename Rhs>
constexpr bool IsESEL<ESELBinExpr<op, Lhs, Rhs>> = true;

struct ESELConstant {
    int val;

    template<typename VarOrderList, typename... Args>
    auto eval(Args... args) -> decltype(auto) {
        return val;
    }
};

template<>
constexpr bool IsESEL<ESELConstant> = true;

template<typename Rhs, typename Lhs>
using ESELSum = ESELBinExpr<ESELOp::Sum, Lhs, Rhs>;

template<typename Rhs, typename Lhs>
using ESELProd = ESELBinExpr<ESELOp::Sum, Lhs, Rhs>;

template<bool negate, typename Rhs, typename Lhs>
using ESELCmp = ESELBinExpr<negate ? ESELOp::Unequal : ESELOp::Equal, Lhs, Rhs>;

template<typename First, typename Rest>
std::enable_if_t<IsESEL<First> && IsESEL<Rest>, ESELBinExpr<ESELOp::Sum, First, Rest>> operator+(const First& x, const Rest& y) {
    return ESELBinExpr<ESELOp::Sum, First, Rest>{ x, y };
}

template<typename First>
ESELBinExpr<ESELOp::Sum, First, ESELConstant> operator+(const First& x, const int& y) {
    return ESELBinExpr<ESELOp::Sum, First, ESELConstant>{ x, ESELConstant{ y } };
}

template<typename First, typename Rest>
std::enable_if_t<IsESEL<First>&& IsESEL<Rest>, ESELBinExpr<ESELOp::Prod, First, Rest>> operator*(const First& x, const Rest& y) {
    return ESELBinExpr<ESELOp::Prod, First, Rest>{ x, y };
}

template<typename First>
ESELBinExpr<ESELOp::Prod, First, ESELConstant> operator*(const First& x, const int& y) {
    return ESELBinExpr<ESELOp::Prod, First, ESELConstant>{ x, ESELConstant{ y } };
}

template<typename First, typename Rest>
std::enable_if_t<IsESEL<First>&& IsESEL<Rest>, ESELBinExpr<ESELOp::Unequal, First, Rest>> operator!=(const First& x, const Rest& y) {
    return ESELBinExpr<ESELOp::Unequal, First, Rest>{ x, y };
}

template<typename First>
ESELBinExpr<ESELOp::Unequal, First, ESELConstant> operator!=(const First& x, const int& y) {
    return ESELBinExpr<ESELOp::Unequal, First, ESELConstant>{ x, ESELConstant{ y } };
}

template<typename First, typename Rest>
std::enable_if_t<IsESEL<First>&& IsESEL<Rest>, ESELBinExpr<ESELOp::Equal, First, Rest>> operator==(const First& x, const Rest& y) {
    return ESELBinExpr<ESELOp::Equal, First, Rest>{ x, y };
}

template<typename First>
ESELBinExpr<ESELOp::Equal, First, ESELConstant> operator==(const First& x, const int& y) {
    return ESELBinExpr<ESELOp::Equal, First, ESELConstant>{ x, ESELConstant{ y } };
}

template<typename Expr, typename... VarOrder>
struct ESELParameterized {
    Expr expr;

    template<typename... Args>
    auto operator()(Args... args) -> decltype(auto) {
        return expr.eval < TList<VarOrder...>, Args... >(args...);
    }
};

#define DEFINE_BIN_OP(OP) \
template<typename E, typename E2, typename... VarOrder> \
auto operator OP(const ESELParameterized<E, VarOrder...>& x, const E2& y) -> decltype(auto) { \
    auto result = x.expr OP y; \
    ESELParameterized<decltype(result), VarOrder...> res{ result }; \
    return res; \
}

DEFINE_BIN_OP(+)
DEFINE_BIN_OP(*)
DEFINE_BIN_OP(==)
DEFINE_BIN_OP(!=)

template<typename E, typename E2, typename... VarOrder>
auto operator|(const ESELParameterized<E, VarOrder...>& x, const E2& y) -> decltype(auto) {
        auto result = x.expr.compose(y);
        ESELParameterized<decltype(result), VarOrder...> res{ result };
        return res;
}

template<typename... VarOrder>
struct AllVars {
    ESELParameterized<Var<0>, VarOrder...> x;
    ESELParameterized<Var<1>, VarOrder...> y;
    ESELParameterized<Var<2>, VarOrder...> z;
};

template<typename... VarOrder>
struct AllOptions : public AllVars<VarOrder...> {
    ESELParameterized<ESELTernaryOpBuilder<>, VarOrder...> choose;
};

template<typename... Vars>
struct Parameters {

    AllOptions<Vars...> params;

    Parameters() {}

    template<typename... Args>
    auto operator->() {
        return &params;
    }
};

template<int id0, int id1>
Parameters<Var<id0>, Var<id1>> operator,(Var<id0> lhs, Var<id1> rhs) {
    return Parameters<Var<id0>, Var<id1>>{};
}

template<int id, typename... Vars>
Parameters<Vars..., Var<id>> operator,(Parameters<Vars...> lhs, Var<id> rhs) {
    return Parameters<Vars..., Var<id>>{};
}

#define if(cond) choose | cond |
#define else |