/*********************************************\
 * 火山视窗PIV模块 - quill日志类             *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_QUILL_HPP
#define _PIV_QUILL_HPP

#include "piv_encoding.hpp"

#ifndef FMT_HEADER_ONLY
    #define FMT_HEADER_ONLY
#endif

#define QUILL_NO_EXCEPTIONS

#if !defined(QUILL_ENABLE_IMMEDIATE_FLUSH)
    #define QUILL_ENABLE_IMMEDIATE_FLUSH 1
#endif

#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_TRACE_L3 0
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_TRACE_L2 1
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_TRACE_L1 2
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_DEBUG 3
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_INFO 4
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_NOTICE 5
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_WARNING 6
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_ERROR 7
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL_CRITICAL 8

#if !defined(QUILL_COMPILE_ACTIVE_LOG_LEVEL)
    #define QUILL_COMPILE_ACTIVE_LOG_LEVEL -1
#endif

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/Logger.h"

#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/FileSink.h"
#include "quill/sinks/RotatingSink.h"

#include "quill/std/WideString.h"
#include "quill/std/Vector.h"

#define QUILL_U8(str) u8##str
#define QUILL_FILE_U8(file, line) u8##file##":"##line
#define QUILL_FUNC_U8(class, method) u8##class##"::"##method

#define QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, log_level, fmt, ...)                \
    do                                                                                                    \
    {                                                                                                     \
        if (logger && logger->template should_log_statement<log_level>())                                 \
        {                                                                                                 \
            static constexpr quill::MacroMetadata macro_metadata{                                         \
                caller_file, caller_function, fmt, tags, log_level,                                       \
                quill::MacroMetadata::Event::Log};                                                        \
            logger->template log_statement<QUILL_ENABLE_IMMEDIATE_FLUSH>(&macro_metadata, ##__VA_ARGS__); \
        }                                                                                                 \
    } while (0)

#define QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, log_level, fmt, ...) \
    do                                                                                                         \
    {                                                                                                          \
        if (logger && logger->template should_log_statement<log_level>())                                      \
        {                                                                                                      \
            thread_local std::chrono::time_point<std::chrono::steady_clock> next_log_time;                     \
            thread_local uint64_t suppressed_log_count{0};                                                     \
            auto const now = std::chrono::steady_clock::now();                                                 \
                                                                                                               \
            if (now < next_log_time)                                                                           \
            {                                                                                                  \
                ++suppressed_log_count;                                                                        \
                break;                                                                                         \
            }                                                                                                  \
                                                                                                               \
            if constexpr (quill::MacroMetadata::_contains_named_args(fmt))                                     \
            {                                                                                                  \
                QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, log_level, fmt " ({occurred}x)", \
                                  ##__VA_ARGS__, suppressed_log_count + 1);                                    \
            }                                                                                                  \
            else                                                                                               \
            {                                                                                                  \
                QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, log_level, fmt " ({}x)",         \
                                  ##__VA_ARGS__, suppressed_log_count + 1);                                    \
            }                                                                                                  \
                                                                                                               \
            next_log_time = now + std::chrono::nanoseconds{min_interval};                                      \
            suppressed_log_count = 0;                                                                          \
        }                                                                                                      \
    } while (0)

#define QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, log_level, fmt, ...) \
    do                                                                                                                  \
    {                                                                                                                   \
        if (logger && logger->template should_log_statement<log_level>())                                               \
        {                                                                                                               \
            thread_local uint64_t call_count = 0;                                                                       \
            thread_local uint64_t next_log_at = 0;                                                                      \
            if (call_count == next_log_at)                                                                              \
            {                                                                                                           \
                QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, log_level, fmt, ##__VA_ARGS__);           \
                next_log_at += n_occurrences;                                                                           \
            }                                                                                                           \
            ++call_count;                                                                                               \
        }                                                                                                               \
    } while (0)

#define QUILL_LOG_BACKTRACE(caller_file, caller_function, logger, tags, fmt, ...)                         \
    do                                                                                                    \
    {                                                                                                     \
        if (logger && logger->template should_log_statement<quill::LogLevel::Backtrace>())                \
        {                                                                                                 \
            static constexpr quill::MacroMetadata macro_metadata{                                         \
                caller_file, caller_function, fmt, tags, quill::LogLevel::Backtrace,                      \
                quill::MacroMetadata::Event::Log};                                                        \
            logger->template log_statement<QUILL_ENABLE_IMMEDIATE_FLUSH>(&macro_metadata, ##__VA_ARGS__); \
        }                                                                                                 \
    } while (0)

#define QUILL_DYNAMIC_LOG(caller_file, caller_line, caller_function, logger, tags, log_level, fmt, ...) \
    QUILL_LOG_RUNTIME_METADATA_SHALLOW(logger, log_level, caller_file, caller_line, caller_function,    \
                                       "", fmt, ##__VA_ARGS__)

#define QUILL_LOG_RUNTIME_METADATA_CALL(event, logger, log_level, file, line_number, function, tags, fmt, ...) \
    do                                                                                                         \
    {                                                                                                          \
        if (logger && logger->should_log_statement(log_level))                                                 \
        {                                                                                                      \
            static constexpr quill::MacroMetadata macro_metadata{                                              \
                "[placeholder]", "[placeholder]", "[placeholder]", nullptr, quill::LogLevel::None, event};     \
                                                                                                               \
            logger->template log_statement_runtime_metadata<QUILL_ENABLE_IMMEDIATE_FLUSH>(                     \
                &macro_metadata, fmt, file, function, tags, line_number, log_level, ##__VA_ARGS__);            \
        }                                                                                                      \
    } while (0)

#define QUILL_LOG_RUNTIME_METADATA_DEEP(logger, log_level, file, line_number, function, tags, fmt, ...)  \
    QUILL_LOG_RUNTIME_METADATA_CALL(quill::MacroMetadata::Event::LogWithRuntimeMetadataDeepCopy, logger, \
                                    log_level, file, line_number, function, tags, fmt, ##__VA_ARGS__)

#define QUILL_LOG_RUNTIME_METADATA_HYBRID(logger, log_level, file, line_number, function, tags, fmt, ...)  \
    QUILL_LOG_RUNTIME_METADATA_CALL(quill::MacroMetadata::Event::LogWithRuntimeMetadataHybridCopy, logger, \
                                    log_level, file, line_number, function, tags, fmt, ##__VA_ARGS__)

#define QUILL_LOG_RUNTIME_METADATA_SHALLOW(logger, log_level, file, line_number, function, tags, fmt, ...)  \
    QUILL_LOG_RUNTIME_METADATA_CALL(quill::MacroMetadata::Event::LogWithRuntimeMetadataShallowCopy, logger, \
                                    log_level, file, line_number, function, tags, fmt, ##__VA_ARGS__)

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_TRACE_L3
    #define QUILL_LOG_TRACE_L3(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::TraceL3, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_TRACE_L3_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::TraceL3, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_TRACE_L3_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::TraceL3, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_TRACE_L3(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_TRACE_L3_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_TRACE_L3_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_TRACE_L2
    #define QUILL_LOG_TRACE_L2(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::TraceL2, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_TRACE_L2_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::TraceL2, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_TRACE_L2_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::TraceL2, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_TRACE_L2(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_TRACE_L2_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_TRACE_L2_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_TRACE_L1
    #define QUILL_LOG_TRACE_L1(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::TraceL1, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_TRACE_L1_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::TraceL1, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_TRACE_L1_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::TraceL1, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_TRACE_L1(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_TRACE_L1_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_TRACE_L1_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_DEBUG
    #define QUILL_LOG_DEBUG(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::Debug, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_DEBUG_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::Debug, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_DEBUG_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::Debug, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_DEBUG(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_DEBUG_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_DEBUG_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_INFO
    #define QUILL_LOG_INFO(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::Info, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_INFO_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::Info, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_INFO_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::Info, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_INFO(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_INFO_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_INFO_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_NOTICE
    #define QUILL_LOG_NOTICE(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::Notice, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_NOTICE_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::Notice, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_NOTICE_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::Notice, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_NOTICE(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_NOTICE_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_NOTICE_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_WARNING
    #define QUILL_LOG_WARNING(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::Warning, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_WARNING_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::Warning, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_WARNING_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::Warning, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_WARNING(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_WARNING_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_WARNING_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_ERROR
    #define QUILL_LOG_ERROR(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::Error, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_ERROR_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::Error, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_ERROR_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::Error, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_ERROR(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_ERROR_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_ERROR_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

#if QUILL_COMPILE_ACTIVE_LOG_LEVEL <= QUILL_COMPILE_ACTIVE_LOG_LEVEL_CRITICAL
    #define QUILL_LOG_CRITICAL(caller_file, caller_function, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL(caller_file, caller_function, logger, tags, quill::LogLevel::Critical, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_CRITICAL_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT(caller_file, caller_function, min_interval, logger, tags, quill::LogLevel::Critical, fmt, ##__VA_ARGS__)

    #define QUILL_LOG_CRITICAL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) \
        QUILL_LOGGER_CALL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, quill::LogLevel::Critical, fmt, ##__VA_ARGS__)
#else
    #define QUILL_LOG_CRITICAL(caller_file, caller_function, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_CRITICAL_LIMIT(caller_file, caller_function, min_interval, logger, tags, fmt, ...) (void)0
    #define QUILL_LOG_CRITICAL_LIMIT_EVERY_N(caller_file, caller_function, n_occurrences, logger, tags, fmt, ...) (void)0
#endif

QUILL_BEGIN_NAMESPACE

template <>
struct Codec<CWString>
{
    static size_t compute_encoded_size(detail::SizeCacheVector& conditional_arg_size_cache, const CWString& arg) noexcept
    {
        return Codec<const wchar_t*>::compute_encoded_size(conditional_arg_size_cache, arg.GetText());
    }

    static void encode(std::byte*& buffer, const detail::SizeCacheVector& conditional_arg_size_cache,
                       uint32_t& conditional_arg_size_cache_index, const CWString& arg) noexcept
    {
        Codec<const wchar_t*>::encode(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, arg.GetText());
    }

    static auto decode_arg(std::byte*& buffer)
    {
        std::wstring_view v = Codec<const wchar_t*>::decode_arg(buffer);
        return detail::utf8_encode(v);
    }

    static void decode_and_store_arg(std::byte*& buffer, DynamicFormatArgStore* args_store)
    {
        args_store->push_back(decode_arg(buffer));
    }
};

template <>
struct Codec<CWConstString>
{
    static size_t compute_encoded_size(detail::SizeCacheVector& conditional_arg_size_cache, const CWConstString& arg) noexcept
    {
        return Codec<const wchar_t*>::compute_encoded_size(conditional_arg_size_cache, arg.GetText());
    }

    static void encode(std::byte*& buffer, const detail::SizeCacheVector& conditional_arg_size_cache,
                       uint32_t& conditional_arg_size_cache_index, const CWConstString& arg) noexcept
    {
        Codec<const wchar_t*>::encode(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, arg.GetText());
    }

    static auto decode_arg(std::byte*& buffer)
    {
        std::wstring_view v = Codec<const wchar_t*>::decode_arg(buffer);
        return detail::utf8_encode(v);
    }

    static void decode_and_store_arg(std::byte*& buffer, DynamicFormatArgStore* args_store)
    {
        args_store->push_back(decode_arg(buffer));
    }
};

class EventSink : public Sink
{
public:
    using EventReceiver_t = std::function<int(const MacroMetadata*, int64_t, std::string_view&, std::string_view&, std::string_view&)>;

    EventSink(EventReceiver_t&& event_receiver) : _event_receiver(event_receiver) {}

    QUILL_ATTRIBUTE_HOT void write_log(const MacroMetadata* log_metadata, uint64_t log_timestamp,
                                       std::string_view thread_id, std::string_view thread_name,
                                       const std::string& process_id, std::string_view logger_name,
                                       LogLevel log_level, std::string_view log_level_description,
                                       std::string_view log_level_short_code,
                                       const std::vector<std::pair<std::string, std::string>>* named_args,
                                       std::string_view log_message, std::string_view log_statement) override
    {
        if (_event_receiver)
            _event_receiver(log_metadata, static_cast<int64_t>(log_timestamp), logger_name, log_message, log_statement);
    }

    QUILL_ATTRIBUTE_HOT void flush_sink() override {}

    QUILL_ATTRIBUTE_HOT void set_event(EventReceiver_t&& event_receiver)
    {
        _event_receiver = event_receiver;
    }

private:
    EventReceiver_t _event_receiver;
};

QUILL_END_NAMESPACE

#endif // _PIV_QUILL_HPP
