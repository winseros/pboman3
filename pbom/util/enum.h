#pragma once

#include <type_traits>
#include <QVariant>

namespace pboman3::util {
    namespace internals {
        template <typename TEnum, TEnum Value0, TEnum... Values>
        struct EnumReader {
            static bool tryRead(int value, TEnum& result) {
                if (value == static_cast<int>(Value0)) {
                    result = static_cast<TEnum>(value);
                    return true;
                }
                if constexpr (sizeof...(Values) > 0) {
                    return EnumReader<TEnum, Values...>::tryRead(value, result);
                }
                return false;
            }
        };
    }

    template <typename TEnum, TEnum... Values>
    bool TryReadEnum(const QVariant& value, TEnum& result) {
        static_assert(std::is_enum_v<TEnum>);
        static_assert(sizeof...(Values) > 0);

        if (value.typeId() != QMetaType::Int)
            return false;

        const auto res = internals::EnumReader<TEnum, Values...>::tryRead(value.toInt(), result);
        return res;
    }
}
