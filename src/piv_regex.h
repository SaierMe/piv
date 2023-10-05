#ifndef _PIV_SIMPLE_REGEX_H
#define _PIV_SIMPLE_REGEX_H

// https://gitee.com/scu319hy/simple_regex

#include <memory.h>
#include <list>
#include <deque>
#include "piv_string.hpp"

#if __cplusplus >= 202002L
#define __SIMPLE_REGEX_LIKELY__		[[likely]]
#define __SIMPLE_REGEX_UNLIKELY__	[[unlikely]]
#else
#define __SIMPLE_REGEX_LIKELY__
#define __SIMPLE_REGEX_UNLIKELY__
#endif

template < typename _TChar = wchar_t >
class regex_reader_t {
	struct reader_position_type {
		using _this_t = reader_position_type;
		uint64_t offset;
		uint32_t line;
		uint32_t col;
		reader_position_type(size_t pos = 0) : offset(pos), line(0), col(0) {}
		bool operator >= (const _this_t&obj) const {
			return offset >= obj.offset;
		}
		bool operator == (const _this_t&obj) const {
			return offset == obj.offset;
		}
		operator uint64_t () const {
			return offset;
		}
		uint64_t line_offset() const {
			return offset - col;
		}
		_this_t &operator ++() {
			offset++;
			col++;
			return *this;
		}
		_this_t operator ++(int32_t) {
			_this_t v(*this);
			offset++;
			col++;
			return v;
		}

	};
public:
	using position_type = reader_position_type;
	using char_type = _TChar;
private:
	position_type _pos;
	uint64_t _max_pos;
	const _TChar *_buffer;
public:
	regex_reader_t(const _TChar *buf) : _pos(0), _max_pos(0), _buffer(buf) {
		while(buf[_max_pos]) {++_max_pos;}
	}
	regex_reader_t(const _TChar *buf, size_t len) : _pos(0), _max_pos(len), _buffer(buf) {}
	uint16_t read() {
		uint16_t c = _buffer[_pos++];
		if (c == '\n') {
			++_pos.line;
			_pos.col = 0;
		}
		return c;
	}
	uint16_t peek() {
		return _buffer[_pos];
	}
	bool end() const {
		return _pos.offset >= _max_pos;
	}
	const position_type &pos() const {
		return _pos;
	}
	bool set_pos(const position_type &pos) {
		_pos = pos;
		return true;
	}
	bool seek(int32_t offset) {
		if (offset == 0) {
			return true;
		}
		if (offset > 0) {
			if (offset + _pos.offset > _max_pos) {
				offset = _max_pos - _pos.offset;
			}
			while(offset --) {
				read();
			}
			return true;
		}

		offset = -offset;
		if (offset <= _pos.col) {
			_pos.offset -= offset;
			_pos.col -= offset;
			return true;
		}
		while (offset > 0) {
			_pos.offset -= _pos.col;
			offset -= _pos.col;
			_pos.col = 0;
			uint64_t line_begin = _pos.offset - 1;
			while (line_begin > 0 && _buffer[--line_begin] == '\n') {}
			_pos.col = _pos.offset - line_begin;
		}
		return true;
	}
	const _TChar *data() const {
		return _buffer;
	}
	const _TChar *end_pos() const {
		return _buffer + _max_pos;
	}
	size_t size() const {
		return static_cast<size_t>(_max_pos);
	}
};

template < typename _TChar = wchar_t >
class regex_fast_reader_t {
public:
	using position_type = const _TChar *;
	using char_type = _TChar;
private:
	const _TChar *_pos;
	const _TChar *_end_pos;
	const _TChar *_buffer;
public:
	regex_fast_reader_t(const _TChar *buf) : _pos(buf), _end_pos(buf), _buffer(buf) {
		while(*_end_pos) {++_end_pos;}
	}
	regex_fast_reader_t(const _TChar *buf, size_t len) : _pos(buf), _end_pos(buf + len), _buffer(buf) {}
	uint16_t read() {
		return *_pos++;
	}
	uint16_t peek() {
		return *_pos;
	}
	bool end() const {
		return _pos >= _end_pos;
	}
	const position_type &pos() const {
		return _pos;
	}
	bool set_pos(const position_type &pos) {
		_pos = pos;
		return true;
	}
	bool seek(int32_t offset) {
		auto new_pos = _pos + offset;
		if (new_pos < _buffer) [[unlikely]] {
			_pos = _buffer;
		} else if (new_pos > _end_pos) [[unlikely]] {
			_pos = _end_pos;
		} else {
			_pos = new_pos;
		}
		return true;
	}
	const _TChar *data() const {
		return _buffer;
	}
	const _TChar *end_pos() const {
		return _end_pos;
	}
	size_t size() const {
		return (_end_pos - _buffer);
	}
};

template <typename _TReader>
class simple_regex_base {
public:
	static const uint32_t RGX_LINE_HEAD = 0x00010000;
	static const uint32_t RGX_WORD_BOUNDRY = 0x00020000;
	using position_type = typename _TReader::position_type;
	using char_type = typename _TReader::char_type;
	bool _single_line = false;
	bool _ignore_case = false;
	struct capture_t {
		virtual const position_type &begin() const = 0;
		virtual const position_type &end() const = 0;
		virtual bool valid() const = 0;
		virtual void reset() = 0;
	};
	virtual bool parse(const char_type *reg, bool no_capture = false, int32_t flag = 0) = 0;
	virtual void clear() = 0;
	virtual void reset(uint32_t status = RGX_LINE_HEAD|RGX_WORD_BOUNDRY) = 0;
	virtual bool search(_TReader &reader) = 0;
	virtual bool match(_TReader &reader) = 0;
	virtual bool full_match(_TReader &reader) {
		return match(reader) && reader.end();
	}
	virtual bool search(_TReader &&reader) {
		return search(reader);
	}
	virtual bool match(_TReader &&reader) {
		return match(reader);
	}
	virtual bool full_match(_TReader &&reader) {
		return match(reader) && reader.end();
	}
	virtual size_t capture_count() const = 0;
	virtual const capture_t &get_capture(size_t idx ) = 0;
	virtual bool valid() const = 0;
	virtual const position_type &begin() const = 0;
	virtual const position_type &end() const = 0;
};

namespace {
	template <typename _TPosition, typename _TChar>
	struct _wrapper_position_type {
		_TPosition _pos;
		uint32_t _ch;
		_wrapper_position_type() : _ch(0) {}
		_wrapper_position_type(const _TPosition &pos, uint32_t c)
			: _pos(pos)
			, _ch(c)
		{}
		operator _TPosition&() {
			return _pos;
		}
		bool operator == (const _TPosition &pos) const {
			return _pos == pos;
		}
		uint32_t load_char() const {
			return _ch;
		}
		void save(const _TPosition &pos, uint32_t c) {
			_pos = pos;
			_ch = c;
		}
	};

	template <typename _TChar>
	struct _wrapper_position_type<const _TChar *, _TChar> {
		using _position_type = const _TChar *;
		_position_type _pos;
		_wrapper_position_type() {}
		_wrapper_position_type(const _position_type &pos, uint32_t c)
			: _pos(pos)
		{}
		operator _position_type&() {
			return _pos;
		}
		bool operator == (const _position_type &pos) const {
			return _pos == pos;
		}
		uint32_t load_char() const {
			return _pos[-1];
		}
		void save(const _position_type &pos, uint32_t c) {
			_pos = pos;
		}
	};
}

template <typename _TReader>
class simple_regex_t : public simple_regex_base<_TReader> {
	using _base_t = simple_regex_base<_TReader>;
	using _this_t = simple_regex_t;
	using _position_type = typename _TReader::position_type;
	using _reg_char_type = typename _TReader::char_type;
	class _reader_wrapper_t {
		_TReader *_reader;
		_this_t &_rgx;
		using _wrapper_position_t = _wrapper_position_type<_position_type, _reg_char_type>;
		_wrapper_position_t _store[512];
		size_t _store_pos;
		uint32_t _last_char;
		_wrapper_position_t &_back() {
			return _store[_store_pos-1];
		}
		void _push_back(const _wrapper_position_t &s) {
			assert(_store_pos<512);
			_store[_store_pos++] = s;
		}
		void _push_back(const _position_type &s, uint32_t c) {
			assert(_store_pos<512);
			_store[_store_pos].save(s, c);
			_store_pos++;
		}
		void _pop_back() {
			assert(_store_pos>0);
			--_store_pos;
		}
		void _clear() {
			_store_pos = 0;
		}
		bool _empty() const {
			return _store_pos == 0;
		}
		static uint32_t _char_type(uint32_t c) {
			if ( c == '\n' || c == '\r' ) {
				return 0;
			}
			// 字符
			if ( (c >= '0' && c <='9') || (c >= 'a' && c <='z') || (c >= 'A' && c <='Z') || c == '_' ) {
				return 1;
			}
			// 空白
			if ( c == ' ' || c == '\t' || c == '\v' ) {
				return 2;
			}
			// 标点符号
			if ( c >= 33 && c <= 126 ) {
				return 3;
			}
			// 控制码
			if ( c < 128 ) {
				return 3;
			}
			// 扩展ASCII
			if ( c < 256 ) {
				return 4;
			}
			// 其它
			return 5;
		}
	public:
		_reader_wrapper_t(_this_t &rgx)
			: _reader(nullptr)
			, _rgx(rgx)
			, _store_pos(0)
			, _last_char(UINT32_MAX)
		{}
		void rebind(_TReader *reader) {
			_reader = reader;
			_store_pos = 0;
		}
		void reset_char() {
			_last_char = UINT32_MAX;
		}
		static bool test_word_boundry(uint16_t c, uint16_t c2) {
			return _char_type(c) != _char_type(c2);
		}
		uint16_t read() {
			return _last_char = _reader->read();
		}
		uint16_t peek() {
			return _reader->peek();
		}
		void skip(size_t n = 1) {
			while(n--) {
				read();
			}
		}
		bool is_word_boundry() {
			if (_last_char == UINT32_MAX) {
				return (_rgx._cur_status&RGX_WORD_BOUNDRY) == RGX_WORD_BOUNDRY;
			}
			if (end()) {
				return true;
			}
			int32_t last_type = _char_type(_last_char);
			return last_type == 0 || last_type != _char_type(_reader->peek());
		}
		bool is_line_head() {
			if (_last_char == UINT32_MAX) {
				return (_rgx._cur_status&RGX_LINE_HEAD) == RGX_LINE_HEAD;
			}
			return _last_char == '\n';
		}
		bool is_line_end() {
			if (_last_char == UINT32_MAX) {
				return (_rgx._cur_status&RGX_LINE_HEAD) == RGX_LINE_HEAD;
			}
			if (end()) {
				return true;
			}
			return peek() == '\n';
		}
		bool end() const {
			return _reader->end();
		}
		void set_pos(const _position_type &pos) {
			_reader->set_pos(pos);
		}
		const _position_type &pos() const {
			return _reader->pos();
		}
		void push_position() {
			_push_back(pos(), _last_char);
		}
		bool pop_position() {
			_wrapper_position_t &last_pos = _back();
			bool changed = !(last_pos == pos());
			_pop_back();
			_reader->set_pos(last_pos);
			_last_char = last_pos.load_char();
			return changed;
		}
		void pop_position_fast() {
			_wrapper_position_t &last_pos = _back();
			_pop_back();
			_reader->set_pos(last_pos);
			_last_char = last_pos.load_char();
		}
		void load_position_fast() {
			_wrapper_position_t &last_pos = _back();
			_reader->set_pos(last_pos);
			_last_char = last_pos.load_char();
		}
		bool drop_position() {
			bool changed = !(_back() == pos());
			_pop_back();
			return changed;
		}
		void drop_position_fast() {
			_pop_back();
		}
		// STACK[... p0 p1] READER[cur] => STACK[... cur] READER[p1]
		bool exchange_drop_prev_position() {
			_position_type cur_pos = pos();
			_wrapper_position_t &last_pos = _back();
			bool changed = !(last_pos == cur_pos);
			_pop_back();
			_pop_back();
			_push_back(cur_pos, _last_char);
			_reader->set_pos(last_pos);
			_last_char = last_pos.load_char();
			return changed;
		}
		// STACK[... p0] READER[cur] => STACK[... cur] READER[p0]
		bool exchange_position() {
			_position_type cur_pos = pos();
			_wrapper_position_t last_pos = _back();
			bool changed = !(last_pos == cur_pos);
			_pop_back();
			_push_back(cur_pos, _last_char);
			_reader->set_pos(last_pos);
			_last_char = last_pos.load_char();
			return changed;
		}
		// STACK[... p0] READER[cur] => STACK[... cur] READER[cur]
		void push_drop_prev_position() {
			_position_type cur_pos = pos();
			_pop_back();
			_push_back(cur_pos, _last_char);
		}
	};

	struct _search_context {
		_position_type &match_begin;
		_position_type &match_end;
	};
public:
	using reader_type = _TReader;
	using position_type = _position_type;
	using char_type = _reg_char_type;
	using _base_t::RGX_LINE_HEAD;
	using _base_t::RGX_WORD_BOUNDRY;

private:
	using _internal_reader_type = _reader_wrapper_t;
	using capture_t = typename _base_t::capture_t;
	// type
	class reg_base {
	protected:
		reg_base * _next;
		virtual bool match_next(_internal_reader_type &reader) {
			if (_next) {
				return _next->match(reader);
			}
			return true;
		}
		virtual bool match_next_no_fallback(_internal_reader_type &reader) {
			if (_next) {
				return _next->match_no_fallback(reader);
			}
			return true;
		}
	public:
		bool _ignore_case{false};
		bool is_fast_search{false};
		reg_base(bool fast_search = false, bool ignore_case = false)
			: _next(0)
			, is_fast_search(fast_search)
			, _ignore_case(ignore_case)
		{}
		virtual ~reg_base() {}
		virtual bool match(_internal_reader_type &) = 0;
		virtual bool match_no_fallback(_internal_reader_type &) = 0;
		virtual bool match_no_fallback(_internal_reader_type &reader, reg_base *ext_next) {
			return this->match_no_fallback(reader);
		}
		virtual void bind_next(reg_base *next) {
			_next = next;
		}
		virtual bool has_next() const {
			return _next != 0;
		}
		virtual reg_base *get_next() const {
			return _next;
		}
		virtual reg_base *set_limit(uint32_t min, uint32_t max = 0xFFFFFFFF) {
			return 0;
		}
		virtual void set_min_match(bool min_match) {}
		virtual bool search(_search_context &ctx, _internal_reader_type &reader) {
			while (!reader.end()) {
				ctx.match_begin = reader.pos();
				if (this->match(reader)) {
					ctx.match_end = reader.pos();
					if (ctx.match_end == ctx.match_begin) {
						reader.skip();
						continue;
					}
					return true;
				}
				reader.skip();
			}
			return false;
		}
		virtual void do_opt(simple_regex_t *rgx) {
			if (_next) {
				_next->do_opt(rgx);
			}
		}
		virtual bool is_same(reg_base *reg) {
			return false;
		}
		virtual uint32_t type() {
			return 0;
		}
		virtual void insert(reg_base *reg, reg_base *with_reg) {
			if (_next == reg) {
				_next = with_reg;
			}
		}
	};

	class reg_string : public reg_base {
		std::vector<uint16_t> _str;
		std::vector<uint16_t> _kmp_next;
		// b is always lower case when _ignore_case == true
		inline bool _cmp(uint16_t a, uint16_t b) {
			if (_ignore_case) {
				// assert(!('A' <= b && b <= 'Z'));
				// return a == b || (b == a + 'a' - 'A' && 'a' <= b && b <= 'z');
				return (a == b) || ((uint16_t)piv::edit::tolower(a) == b);
			}
			return a == b;
		};
		bool _init_next(const std::vector<uint16_t> &pat, std::vector<uint16_t>&next) {
			size_t text_len = pat.size();
			next.resize(text_len);
			uint16_t check_pos = 0;
			// cal next
			next[0] = 0;
			for (size_t i = 1; i < text_len; i++) {
				if (_cmp(pat[check_pos], pat[i])) {
					next[i] = next[check_pos];
					check_pos++;
				} else {
					next[i] = check_pos;
					check_pos = next[check_pos];
					while (!_cmp(pat[check_pos], pat[i]) && check_pos != 0) {
						check_pos = next[check_pos];
					}
					if (_cmp(pat[check_pos], pat[i])) {
						check_pos++;
					}
				}
			}
			return true;
		}
	public:
		reg_string(bool ignore_case = false) : reg_base(true, ignore_case) { }

		bool match(_internal_reader_type &reader) override {
			if (reader.end() || reader.peek() != _str[0]) {
				return false;
			}
			reader.push_position();
			auto c = _str.data();
			auto str_end = _str.data() + _str.size();
			while (c != str_end) {
				if (_cmp(reader.read(), *c)) {
					++c;
					continue;
				}
				reader.pop_position_fast();
				return false;
			}
			if (this->match_next_no_fallback(reader)) {
				reader.drop_position_fast();
				return true;
			}
			reader.pop_position_fast();
			return false;
		}

		bool match_no_fallback(_internal_reader_type &reader) override {
			if (reader.end()) {
				return false;
			}
			auto c = _str.data();
			auto str_end = _str.data() + _str.size();
			while (c != str_end) {
				if (_cmp(reader.read(), *c)) {
					++c;
					continue;
				}
				return false;
			}
			if (this->match_next_no_fallback(reader)) {
				return true;
			}
			return false;
		}

		void append(uint16_t c) {
			if (_ignore_case) {
				c = (uint16_t)piv::edit::tolower(c);
			}
			_str.push_back( c );
		}

		bool search(_search_context &ctx, _internal_reader_type &reader) override {
			auto kmp_str = get_str();
			auto kmp_next = get_kmp_next();
			auto str_begin = kmp_str.data();
			auto c = str_begin;
			auto str_end = kmp_str.data() + kmp_str.size();
			ctx.match_begin = reader.pos();

			while (true) {
				if (c == str_end) {
					// matched
					if (this->match_next(reader)) {
						// next pattern matched
						ctx.match_end = reader.pos();
						return true;
					}
					// next pattern not matched
					uint16_t next_pos = kmp_next.back();
					if (next_pos == 0) {
						c = str_begin;
						ctx.match_begin = reader.pos();
						continue;
					}
					reader.set_pos(ctx.match_begin);
					reader.skip(kmp_next.size() - next_pos);
					ctx.match_begin = reader.pos();
					reader.skip(next_pos - 1);
					c = str_begin + next_pos;
					continue;
				}
				if (reader.end()) {
					return false;
				}
				auto ch = reader.read();
__match_again:
				if (_cmp(ch, *c)) {
					++c;
					continue;
				}
				auto offset = c - str_begin;
				uint16_t next_pos;
				if (offset == 0 || (next_pos = kmp_next[offset]) == 0) {
					c = str_begin;
					ctx.match_begin = reader.pos();
					continue;
				}
				{
					auto current_pos = reader.pos();
					reader.set_pos(ctx.match_begin);
					reader.skip(offset - next_pos);
					ctx.match_begin = reader.pos();
					reader.set_pos(current_pos);
					c = str_begin + next_pos;
					goto __match_again;
				}
			}
			return false;
		}

		void do_opt(simple_regex_t *rgx) override {
			reg_base::do_opt(rgx);
		}

		const std::vector<uint16_t> &get_str() const {
			return _str;
		}

		const std::vector<uint16_t> &get_kmp_next() {
			if (_str.size() != _kmp_next.size()) {
				_init_next(_str, _kmp_next);
			}
			return _kmp_next;
		}

		uint32_t type() override {
			return ID;
		}

		static constexpr uint32_t ID = (__COUNTER__ + 1)* 4;
	};

	template <bool _IsMinMatch, typename _TImpl>
	class reg_limit_base_impl : public reg_base {
		uint32_t _min, _max;

		std::shared_ptr<std::deque<position_type>> _marks;

		bool _match_no_next(_internal_reader_type &reader) {
			if (reader.end()) {
				return _min == 0;
			}
			uint32_t match_count = 0;
			reader.push_position();
			while (true) {
				if (match_count >= _min) {
					if (_IsMinMatch) {
						reader.drop_position_fast();
						return true;
					} else {
						reader.exchange_position();
						goto __result_has_found;
					}
					if (match_count >= _max) {
						break;
					}
				}
				if (!static_cast<_TImpl*>(this)->do_match(reader)) {
					break;
				}
				++match_count;
			}
			reader.pop_position_fast();
			return false;
			while (true) {
				assert(!_IsMinMatch);
				reader.exchange_position();
__result_has_found:
				if (match_count >= _max) {
					break;
				}
				if (!static_cast<_TImpl*>(this)->do_match(reader)) {
					break;
				}
				++match_count;
			}
			reader.pop_position_fast();
			return true;
		}

		bool _match_with_next(_internal_reader_type &reader, reg_base *next) {
			if (reader.end()) {
				if (_min == 0) {
					return next->match(reader);
				}
				return false;
			}
			uint32_t match_count = 0;
			reader.push_position();
			while (true) {
				if (match_count>=_min) {
					if (_IsMinMatch) {
						if (next->match(reader)) {
							reader.drop_position_fast();
							return true;
						}
					} else {
						reader.push_position();
						if (next->match_no_fallback(reader)) {
							reader.exchange_drop_prev_position();
							goto __result_has_found;
						} else {
							reader.pop_position_fast();
						}
					}
					if (match_count >= _max) {
						break;
					}
				}
				if (!static_cast<_TImpl*>(this)->do_match(reader)) {
					break;
				}
				++match_count;
			}
			reader.pop_position_fast();
			return false;

			while (true) {
				reader.push_position();
				if (next->match_no_fallback(reader)) {
					reader.exchange_drop_prev_position();
				} else {
					reader.pop_position_fast();
				}
__result_has_found:
				if (match_count >= _max) {
					break;
				}
				if (!static_cast<_TImpl*>(this)->do_match(reader)) {
					break;
				}
				++match_count;
			}
			reader.pop_position_fast();
			return true;
		}

		bool _match_with_next(_internal_reader_type &reader, reg_base *next, reg_base *ext_next) {
			if (reader.end()) {
				if (_min == 0) {
					reader.push_position();
					if (next->match_no_fallback(reader, ext_next)) {
						reader.drop_position_fast();
						return true;
					}
					reader.drop_position_fast();
				}
				return false;
			}
			uint32_t match_count = 0;
			uint32_t success_count = 0;
			reader.push_position();
			while (true) {
				if (match_count>=_min) {
					reader.push_position();
					if (next->match_no_fallback(reader)) {
						reader.push_position();
						if (ext_next->match(reader)) {
							if (_IsMinMatch) {
								reader.pop_position_fast();
								reader.drop_position_fast();
								reader.drop_position_fast();
								return true;
							}
							reader.pop_position_fast();
							reader.exchange_drop_prev_position();
							goto __result_has_found;
						} else {
							if (success_count++ == 0) {
								// save the first match point
								reader.pop_position_fast();
								reader.exchange_drop_prev_position();
							} else {
								reader.drop_position_fast();
								reader.pop_position_fast();
							}
						}
					} else {
						reader.pop_position_fast();
					}
					if (match_count >= _max) {
						break;
					}
				}
				if (!static_cast<_TImpl*>(this)->do_match(reader)) {
					break;
				}
				++match_count;
			}
			reader.pop_position_fast();
			return success_count > 0;

			while (true) {
				reader.push_position();
				if (next->match_no_fallback(reader)) {
					reader.push_position();
					if (ext_next->match(reader)) {
						reader.pop_position_fast();
						reader.push_drop_prev_position();
					} else {
						reader.drop_position_fast();
						reader.pop_position_fast();
					}
				} else {
					reader.pop_position_fast();
				}
__result_has_found:
				if (match_count >= _max) {
					break;
				}
				if (!static_cast<_TImpl*>(this)->do_match(reader)) {
					break;
				}
				++match_count;
			}
			reader.pop_position_fast();
			return true;
		}

		// return 0 for success, other value for skip a certain characters
		inline int32_t _kmp_test(_internal_reader_type &reader, const std::vector<uint16_t> &kmp_str, const std::vector<uint16_t> &kmp_next) {
			auto kmp_begin = kmp_str.data();
			auto kmp_end = kmp_begin + kmp_str.size();
			auto c = kmp_begin;
			auto cmp = [](uint16_t a, uint16_t b, bool ignore_case = false) {
				if (ignore_case) {
					//assert(!('A' <= b && b <= 'Z'));
					//return a == b || (b == a + 'a' - 'A' && 'a' <= b && b <= 'z');
					return (a == b) || ((uint16_t)piv::edit::tolower(a) == b);
				}
				return a == b;
			};

			while (true) {
				if (c == kmp_end) {
					return 0;
				}

				auto ch = reader.read();
				if (cmp(ch, *c, _ignore_case)) {
					++c;
					continue;
				}

				auto offset = c - kmp_begin;
				if (offset == 0) {
					return 1;
				}
				auto next_pos = kmp_next[offset];
				return int32_t(offset - next_pos);
			}
		}
	public:
		reg_limit_base_impl() {
		}

		reg_limit_base_impl(uint32_t min_count, uint32_t max_count, bool ignore_case = false)
			: reg_base(false, ignore_case)
			, _min(min_count)
			, _max(max_count)
		{}

		reg_limit_base_impl(bool has_fast_search, uint32_t min_count, uint32_t max_count, bool ignore_case = false)
		: reg_base(has_fast_search, ignore_case)
		, _min(min_count)
		, _max(max_count)
		{}

		bool match(_internal_reader_type &reader) override {
			auto next = this->get_next();
			if (next) {
				return _match_with_next(reader, next);
			}
			return _match_no_next(reader);
		}

		bool match_no_fallback(_internal_reader_type &reader) override {
			auto next = this->get_next();
			if (next) {
				return _match_with_next(reader, next);
			}
			return _match_no_next(reader);
		}

		bool match_no_fallback(_internal_reader_type &reader, reg_base *ext_next) override {
			if (_IsMinMatch) {
				auto next = this->get_next();
				if (next) {
					return _match_with_next(reader, next);
				}
				return _match_no_next(reader);
			}
			auto next = this->get_next();
			if (next) {
				if (ext_next) {
					return _match_with_next(reader, next, ext_next);
				} else {
					return _match_with_next(reader, next);
				}
			}
			if (ext_next) {
				return _match_with_next(reader, ext_next);
			}
			return _match_no_next(reader);
		}

		void set_min_match(bool min_match) override {
			if (min_match != _IsMinMatch) {
				_TImpl::rebuild(static_cast<_TImpl*>(this), min_match);
			}
		}

		bool search(_search_context &ctx, _internal_reader_type &reader) override {
			if (_TImpl::do_match_length == 1) {
				return search_fixed_length(ctx, reader);
			}
			return search_var_length(ctx, reader);
		}

		bool search_with_kmp(_search_context &ctx, _internal_reader_type &reader, const std::vector<uint16_t> &kmp_str, const std::vector<uint16_t> &kmp_next, reg_base *next) {
			uint32_t match_count = 0;
			int32_t skip_count = 0;
			uint32_t stored_match_count = 0;
__search_begin:
			ctx.match_begin = reader.pos();
			if (reader.end()) {
				if (_min == 0 && this->match_next(reader)) {
					ctx.match_end = reader.pos();
					return ctx.match_begin != ctx.match_end;
				}
				return false;
			}
			while (true) {
				if (match_count>=_min && skip_count-- <= 0) {
					reader.push_position();
					skip_count = _kmp_test(reader, kmp_str, kmp_next);
					if (skip_count != 0) {
						reader.pop_position_fast();
						continue;
					}
					skip_count = int32_t(kmp_next.size() - 1 - kmp_next.back());
					if (!next || next->match_no_fallback(reader)) {
						stored_match_count = match_count;
						reader.exchange_position();
						if (_IsMinMatch) {
							goto __match_end;
						} else {
							goto __result_has_found;
						}
					} else {
						reader.pop_position_fast();
					}
				}
				if (reader.end()) {
					goto __search_begin;
				}
				uint16_t value = reader.read();
				if (!static_cast<_TImpl*>(this)->match(value)) {
					match_count = 0;
					goto __search_begin;	// try again
				}
				++match_count;
			}
			return false;

			while (true) {
				if (skip_count-- <= 0) {
					reader.push_position();
					skip_count = _kmp_test(reader, kmp_str, kmp_next);
					if (skip_count != 0) {
						reader.pop_position_fast();
						continue;
					}
					skip_count = int32_t(kmp_next.size() - 1 - kmp_next.back());

					if (!next || next->match_no_fallback(reader)) {
						stored_match_count = match_count;
						reader.exchange_drop_prev_position();
						assert(!_IsMinMatch);
					} else {
						reader.pop_position_fast();
					}
__result_has_found:
					if (match_count >= _max) {
						break;
					}
				}
				if (reader.end()) {
					break;
				}
				uint16_t value = reader.read();
				if (!static_cast<_TImpl*>(this)->match(value)) {
					break;
				}
				++match_count;
			}
__match_end:
			if (_IsMinMatch && stored_match_count > _min) {
				reader.set_pos(ctx.match_begin);
				reader.skip(stored_match_count - _min);
				ctx.match_begin = reader.pos();
			} else if (stored_match_count > _max) {
				reader.set_pos(ctx.match_begin);
				reader.skip(stored_match_count - _max);
				ctx.match_begin = reader.pos();
			}
			reader.pop_position_fast();
			ctx.match_end = reader.pos();
			return true;
		}

		bool search_fixed_length(_search_context &ctx, _internal_reader_type &reader) {
			auto next = this->get_next();
			if (next && next->type() == reg_string::ID) {
				auto next_str = static_cast<reg_string*>(next);
				auto &kmp_str = next_str->get_str();
				auto &kmp_next = next_str->get_kmp_next();
				return search_with_kmp(ctx, reader, kmp_str, kmp_next, next->get_next());
			}
			return search_var_length(ctx, reader);
		}

		bool search_var_length(_search_context &ctx, _internal_reader_type &reader) {
			if (!_marks) {
				_marks = std::make_shared<std::deque<position_type>>();
			}
			std::deque<position_type> &marks = *_marks;
			marks.clear();

			uint32_t match_count = 0;
			auto next = this->get_next();
__search_begin:
			ctx.match_begin = reader.pos();
			if (reader.end()) {
				if (_min == 0 && (!next || next->match(reader))) {
					ctx.match_end = reader.pos();
					return ctx.match_begin != ctx.match_end;
				}
				return false;
			}
			while (true) {
				if (match_count>=_min) {
					if (_IsMinMatch) {
						if (!next || next->match(reader)) {
							ctx.match_end = reader.pos();
							if (ctx.match_begin != ctx.match_end) {
								return true;
							}
							// skip the zero length match
						}
					} else {
						reader.push_position();
						if (!next || next->match_no_fallback(reader)) {
							reader.exchange_position();
							marks.clear();
							goto __result_has_found;
						} else {
							reader.pop_position_fast();
						}
					}
				}
				if (match_count == 0 && _TImpl::has_do_search) {
					if (!static_cast<_TImpl*>(this)->do_search(ctx, reader)) {
						return false;
					}
				} else if (!static_cast<_TImpl*>(this)->do_match(reader)) {
					if (marks.empty()) {
						reader.set_pos(ctx.match_begin);
					} else {
						reader.set_pos(marks.back());
					}
					marks.clear();
					reader.skip(); // skip one char
					match_count = 0;
					goto __search_begin;	// try again
				}
				if (_max != UINT32_MAX) {
					marks.emplace_back(reader.pos());
				}
				++match_count;
				if (match_count > _max) {
					// skip the first mark
					ctx.match_begin = marks.front();
					marks.pop_front();
					--match_count;
				}
			}
			return false;

			while (true) {
				if (_IsMinMatch) {
					if (!next || next->match(reader)) {
						reader.drop_position_fast();
						ctx.match_end = reader.pos();
						return true;
					}
				} else {
					reader.push_position();
					if (!next || next->match_no_fallback(reader)) {
						reader.exchange_drop_prev_position();
					} else {
						reader.pop_position_fast();
					}
				}
__result_has_found:
				if (match_count >= _max) {
					break;
				}
				if (!static_cast<_TImpl*>(this)->do_match(reader)) {
					break;
				}
				++match_count;
			}
			reader.pop_position_fast();
			ctx.match_end = reader.pos();
			return true;
		}
	};

	template <bool _IsMinMatch>
	class reg_limit : public reg_limit_base_impl<_IsMinMatch, reg_limit<_IsMinMatch> > {
		using _this_t = reg_limit;
		using _base_t = reg_limit_base_impl<_IsMinMatch, _this_t>;
		reg_base * _reg;
	public:
		enum {has_do_search = 1};
		enum {do_match_length = UINT32_MAX};

		reg_limit() {
		}

		reg_limit(bool ignore_case, reg_base *reg, uint32_t min_count, uint32_t max_count = 0xFFFFFFFF)
			: _base_t(min_count, max_count, ignore_case)
			, _reg(reg)
		{}

		bool do_search(_search_context &ctx, _internal_reader_type &reader) {
			return _reg->search(ctx, reader);
		}

		bool do_match(_internal_reader_type &reader) {
			return _reg->match_no_fallback(reader, this->get_next());
		}

		bool match(uint16_t value) {
			return false;
		}

		static void rebuild(_this_t *pthis, bool min_match) {
			if (min_match) {
				new(pthis) reg_limit<true>();
			} else {
				new(pthis) reg_limit<false>();
			}
		}

		void do_opt(simple_regex_t *rgx) override {
			_reg->do_opt(rgx);
			_base_t::do_opt(rgx);
		}
		void insert(reg_base *reg, reg_base *with_reg) override {
			if (_reg == reg) {
				_reg = with_reg;
			}
			_base_t::insert(reg, with_reg);
		}
	};

	template <typename _TImpl>
	class reg_simple_match_impl : public reg_base {
	public:
		reg_simple_match_impl() {}

		reg_simple_match_impl(bool fast_search) : reg_base(fast_search) {}

		bool match(_internal_reader_type &reader) override {
			if (reader.end()) {
				return false;
			}
			uint16_t value = reader.peek();
			if (!static_cast<_TImpl*>(this)->_match(value)) {
				return false;
			}
			reader.push_position();
			reader.skip();
			if (this->match_next_no_fallback(reader)) {
				reader.drop_position_fast();
				return true;
			}
			reader.pop_position_fast();
			return false;
		}

		bool match_no_fallback(_internal_reader_type &reader) override {
			if (reader.end()) {
				return false;
			}
			uint16_t value = reader.read();
			if (!static_cast<_TImpl*>(this)->_match(value)) {
				return false;
			}
			if (this->match_next_no_fallback(reader)) {
				return true;
			}
			return false;
		}

		bool search(_search_context &ctx, _internal_reader_type &reader) override {
			while (!reader.end()) {
				ctx.match_begin = reader.pos();
				uint16_t value = reader.read();
				if (!static_cast<_TImpl*>(this)->_match(value)) {
					continue;
				}
				auto next = this->get_next();
				if (!next) {
					goto __matched;
				}
				if (next->is_fast_search) {
					auto initial_begin = ctx.match_begin;
					auto search_begin = reader.pos();
					if (!next->search(ctx, reader)) {
						return false;
					}
					if (search_begin == ctx.match_begin) {
						ctx.match_begin = initial_begin;
						goto __matched;
					}
					reader.set_pos(search_begin);
					_position_type last_pos, cur_pos;
					uint16_t last_value;
					value = reader.peek();
					while((cur_pos = reader.pos()) != ctx.match_begin) {
						last_pos = cur_pos;
						last_value = value;
						value = reader.read();
					}
					if (!static_cast<_TImpl*>(this)->_match(value)) {
						continue;
					}
					// matched
					ctx.match_begin = last_pos;
					reader.set_pos(ctx.match_end);
					return true;
				}
				if (next->match_no_fallback(reader)) {
					goto __matched;
				}
				reader.set_pos(ctx.match_begin);
				reader.skip();
				continue;
__matched:
				ctx.match_end = reader.pos();
				return true;
			}
			return false;
		}
	};

	class reg_char_set : public reg_simple_match_impl<reg_char_set> {
	protected:
		static constexpr uint32_t ESZ = 8*sizeof(size_t);
		size_t _map[0x10000/ESZ];
		inline bool _match(uint16_t value) {
			uint32_t idx = value/ESZ;
			uint32_t pos = value%ESZ;
			return (_map[idx]>>pos)&0x01;
		}
		friend class reg_simple_match_impl<reg_char_set>;
	public:
		virtual void append(uint16_t c) = 0;

		virtual void append(uint16_t begin, uint16_t end) = 0;

		virtual reg_base * shink() = 0;
	};

	template <bool _IsMinMatch, typename _TPolicy>
	class reg_limited_impl : public reg_limit_base_impl<_IsMinMatch, reg_limited_impl<_IsMinMatch, _TPolicy> > {
		using _this_t = reg_limited_impl;
		using _base_t = reg_limit_base_impl<_IsMinMatch, _this_t>;
		_TPolicy _policy;
	public:
		enum {has_do_search = _TPolicy::has_do_search};
		enum {do_match_length = 1};

		reg_limited_impl() {}

		reg_limited_impl(uint32_t min_count, uint32_t max_count)
			: _base_t(min_count, max_count)
		{}

		reg_limited_impl(_TPolicy &&policy, uint32_t min_count, uint32_t max_count)
		: _base_t(min_count, max_count)
		, _policy{policy}
		{}

		bool do_search(_search_context &ctx, _internal_reader_type &reader) {
			return _policy.do_search(ctx, reader);
		}

		bool do_match(_internal_reader_type &reader) {
			if (reader.end()) {
				return false;
			}
			return _policy.do_match(reader);
		}

		bool match(uint16_t value) {
			return _policy.match(value);
		}

		static void rebuild(_this_t *pthis, bool min_match) {
			if (min_match) {
				new(pthis) reg_limited_impl<true, _TPolicy>();
			} else {
				new(pthis) reg_limited_impl<false, _TPolicy>();
			}
		}

		void do_opt(simple_regex_t *rgx) override {
			_policy.do_opt(rgx);
			_base_t::do_opt(rgx);
		}
	};

	template<bool _IsInverse>
	class reg_char_set_t : public reg_char_set {
		bool _ignore_case = false;
		struct char_set_policy {
			size_t *_map;

			enum {has_do_search = 0};

			void do_opt(simple_regex_t *rgx) {}

			bool do_search(_search_context &ctx, _internal_reader_type &reader) {
				return false;
			}

			bool do_match(_internal_reader_type &reader) {
				return match(reader.read());
			}

			bool match(uint16_t value) {
				static constexpr uint32_t ESZ = 8*sizeof(size_t);
				uint32_t idx = value/ESZ;
				uint32_t pos = value%ESZ;
				return (_map[idx]>>pos)&0x01;
			}
		};
		void _append(uint16_t c) {
			uint32_t idx = c/reg_char_set::ESZ;
			uint32_t pos = c%reg_char_set::ESZ;
			if (_IsInverse) {
				this->_map[idx] &= ~(size_t(1)<<pos);
			} else {
				this->_map[idx] |= (size_t(1)<<pos);
			}
		}

		template<int _ElementCount>
		struct small_char_set : public reg_simple_match_impl<small_char_set<_ElementCount>> {
			using _base_t = reg_simple_match_impl<small_char_set>;
			uint16_t _n;
			uint16_t _items[_ElementCount];
			inline bool _match(uint16_t value) {
				if (_ElementCount > 0 && _items[0] == value) __SIMPLE_REGEX_UNLIKELY__ {
					return !_IsInverse;
				}
				if (_ElementCount > 1 && _items[1] == value) __SIMPLE_REGEX_UNLIKELY__ {
					return !_IsInverse;
				}
				if (_ElementCount > 2 && _items[2] == value) __SIMPLE_REGEX_UNLIKELY__ {
					return !_IsInverse;
				}
				if (_ElementCount > 3 && _items[3] == value) __SIMPLE_REGEX_UNLIKELY__ {
					return !_IsInverse;
				}
				if (_ElementCount > 4 && _items[4] == value) __SIMPLE_REGEX_UNLIKELY__ {
					return !_IsInverse;
				}
				if (_ElementCount > 5 && _items[5] == value) __SIMPLE_REGEX_UNLIKELY__ {
					return !_IsInverse;
				}
				return _IsInverse;
			}
			friend class reg_simple_match_impl<small_char_set>;

			small_char_set(const uint16_t *e, reg_base *next) : _base_t(true) {
				_n = _ElementCount;
				memcpy(_items, e, sizeof(uint16_t)*_ElementCount);
				if (next) {
					this->bind_next(next);
				}
			}

			bool is_same(reg_base *reg) override {
				if (this->type() != reg->type()) {
					return false;
				}
				if (_ElementCount != static_cast<small_char_set*>(reg)->_n) {
					return false;
				}
				if (memcmp(_items, static_cast<small_char_set*>(reg)->_items, sizeof(uint16_t)*_ElementCount)) {
					return false;
				}
				return true;
			}

			uint32_t type() override {
				return ID;
			}

			struct small_char_set_policy {
				small_char_set *_set;

				enum {has_do_search = 0};

				void do_opt(simple_regex_t *rgx) {}

				bool do_search(_search_context &ctx, _internal_reader_type &reader) {
					return false;
				}

				bool do_match(_internal_reader_type &reader) {
					return match(reader.read());
				}

				bool match(uint16_t value) {
					return _set->_match(value);
				}
			};

			reg_base *set_limit(uint32_t min, uint32_t max) override {
				return new reg_limited_impl<false, small_char_set_policy>({this}, min, max);
			}

			static constexpr uint32_t ID = (__COUNTER__ + 1)* 4 + _IsInverse;
		};
	public:
		reg_char_set_t(bool ignore_case = false) : _ignore_case(ignore_case) {
			if (_IsInverse) {
				memset(this->_map, 0xFF, sizeof(this->_map));
			} else {
				memset(this->_map, 0x00, sizeof(this->_map));
			}
		}

		void append(uint16_t c) override {
			_append(c);
			if (_ignore_case && c >= 'A' && c <= 'Z') {
				_append(c - 'A' + 'a');
			}
		}

		void append(uint16_t begin, uint16_t end) override {
			for (auto c = begin; c <= end; ++c) {
				_append(c);
				if (_ignore_case && c >= 'A' && c <= 'Z') {
					_append(c - 'A' + 'a');
				}
			}
		}

		reg_base *set_limit(uint32_t min, uint32_t max) override {
			return new reg_limited_impl<false, char_set_policy>({this->_map}, min, max);
		}

		bool is_same(reg_base *reg) override {
			if (this->type() != reg->type()) {
				return false;
			}
			if (memcmp(this->_map, static_cast<reg_char_set_t*>(reg)->_map, sizeof(this->_map))) {
				return false;
			}
			return true;
		}

		reg_base * shink() override {
			uint16_t elements[8];
			uint16_t count = 0;
			for (size_t idx=0; idx<0x10000 && count < 8; ++idx) {
				if (this->_match(uint16_t(idx)) == _IsInverse) {
					continue;
				}
				elements[count] = uint16_t(idx);
				++count;
			}
			reg_base * scs = nullptr;
			switch(count) {
			case 1:
				scs = new small_char_set<1>(elements, this->get_next());
				break;
			case 2:
				scs = new small_char_set<2>(elements, this->get_next());
				break;
			case 3:
				scs = new small_char_set<3>(elements, this->get_next());
				break;
			case 4:
				scs = new small_char_set<4>(elements, this->get_next());
				break;
			case 5:
				scs = new small_char_set<5>(elements, this->get_next());
				break;
			case 6:
				scs = new small_char_set<6>(elements, this->get_next());
				break;
			}
			if (scs) {
				delete this;
				return scs;
			}
			return this;
		}

		uint32_t type() override {
			return ID;
		}

		static constexpr uint32_t ID = (__COUNTER__ + 1)* 4 + _IsInverse;
	};

	template<bool _IsInverse>
	class reg_simple_range : public reg_simple_match_impl<reg_simple_range<_IsInverse> > {
		bool _ignore_case = false;
		struct simple_range_policy {
			uint16_t _begin;
			uint16_t _end;
			bool _ignore_case = false;

			enum {has_do_search = 0};
			void do_opt(simple_regex_t *rgx) {}

			bool do_search(_search_context &ctx, _internal_reader_type &reader) {
				return false;
			}

			bool do_match(_internal_reader_type &reader) {
				return match(reader.read());
			}

			bool match(uint16_t value) {
				if (_ignore_case) {
					value = (uint16_t)piv::edit::tolower(value);
				}
				bool matched = _begin <= value && value <= _end;
				return matched != _IsInverse;
			}
		};

		uint16_t _begin;
		uint16_t _end;
		inline bool _match(uint16_t value) {
			if (_ignore_case) {
				value = (uint16_t)piv::edit::tolower(value);
			}
			return (_begin <= value && value <= _end) != _IsInverse;
		}
		friend class reg_simple_match_impl<reg_simple_range<_IsInverse> >;
	public:
		reg_simple_range(uint16_t begin, uint16_t end, bool ignore_case = false)
			: _begin(begin)
			, _end(end)
			, _ignore_case(ignore_case)
		{
		}

		reg_base *set_limit(uint32_t min, uint32_t max) override {
			return new reg_limited_impl<false, simple_range_policy>({_begin, _end, _ignore_case}, min, max);
		}

		void do_opt(simple_regex_t *rgx) override {
			reg_base::do_opt(rgx);
		}
	};

	template<bool _IsInverse, typename _TMatchPolicy>
	class reg_chars_impl : public reg_simple_match_impl<reg_chars_impl<_IsInverse, _TMatchPolicy> > {
		struct chars_policy {
			enum {has_do_search = 0};
			void do_opt(simple_regex_t *rgx) {}

			bool do_search(_search_context &ctx, _internal_reader_type &reader) {
				return false;
			}

			bool do_match(_internal_reader_type &reader) {
				return match(reader.read());
			}

			bool match(uint16_t value) {
				return _TMatchPolicy::match(value) != _IsInverse;
			}
		};
		inline bool _match(uint16_t value) {
			return _TMatchPolicy::match(value) != _IsInverse;
		}
		friend class reg_simple_match_impl<reg_chars_impl<_IsInverse, _TMatchPolicy> >;
	public:
		reg_chars_impl() {}

		reg_base *set_limit(uint32_t min, uint32_t max) override {
			return new reg_limited_impl<false, chars_policy>(min, max);
		}

		void do_opt(simple_regex_t *rgx) override {
			reg_base::do_opt(rgx);
		}
	};

	struct any_chars {
		static inline bool match(uint16_t value) {
			return value != '\n';
		}
	};

	struct any_chars_single_line {
		static inline bool match(uint16_t value) {
			return true;
		}
	};

	struct space_chars {
		static inline bool match(uint16_t value) {
			return (value >= 0x09 && value <= 0x0d) || value == ' ';
		}
	};

	struct word_chars {
		static inline bool match(uint16_t value) {
			// (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z') || (value >= '0' && value <= '9') || value == '_';
			const uint64_t WORD_CHAR_BITS[2] = {
				0x03FF000000000000ULL,
				0x07FFFFFE87FFFFFEULL,
			};
			if (value > 'z') {
				return false;
			}
			return WORD_CHAR_BITS[value/64]&(1ULL<<(value%64));
		}
	};

	using reg_any = reg_chars_impl<false, any_chars>;

	using reg_any_single_line = reg_chars_impl<false, any_chars_single_line>;

	template<bool _IsInverse>
	using reg_space_chars = reg_chars_impl<_IsInverse, space_chars>;

	template<bool _IsInverse>
	using reg_word_chars = reg_chars_impl<_IsInverse, word_chars>;

	class reg_group : public reg_base {
		reg_base * _first;
		reg_base * _last;
	public:
		reg_group()
			: _first(0)
			, _last(0)
		{}
		void append(reg_base *reg) {
			if ( _first == 0 ) {
				_first = _last = reg;
			} else {
				_last->bind_next(reg);
				_last = reg;
			}
		}
		bool match(_internal_reader_type &reader) override {
			return _first->match(reader);
		}
		bool match_no_fallback(_internal_reader_type &reader) override {
			return _first->match_no_fallback(reader);
		}
		bool match_no_fallback(_internal_reader_type &reader, reg_base *ext_next) {
			return _first->match_no_fallback(reader, ext_next);
		}
		void bind_next(reg_base *next) override {
			_last->bind_next(next);
			reg_base::bind_next(next);
		}
		bool search(_search_context &ctx, _internal_reader_type &reader) override {
			return _first->search(ctx, reader);
		}
		void do_opt(simple_regex_t *rgx) override {
			_first->do_opt(rgx);
		}
		void insert(reg_base *reg, reg_base *with_reg) override {
			if (_first == reg) {
				_first = with_reg;
			}
			reg_base::insert(reg, with_reg);
		}
		reg_base *first() const {
			return _first;
		}
		reg_base *last() const {
			return _last;
		}
		uint32_t type() override {
			return ID;
		}
		static constexpr uint32_t ID = (__COUNTER__ + 1)* 4;
	};

	class reg_capture : public reg_base, public capture_t {
		class _place_holder_t : public reg_base {
			_position_type &_pos;
		public:
			_place_holder_t(_position_type &pos) : _pos(pos) {}
			void append(reg_base *reg) {
				assert("should not run this function!" && 0);
			}
			bool match(_internal_reader_type &reader) {
				_position_type pos = reader.pos();
				if (this->match_next(reader)) {
					_pos = pos;
					return true;
				}
				return false;
			}
			bool match_no_fallback(_internal_reader_type &reader) {
				_position_type pos = reader.pos();
				if (this->match_next_no_fallback(reader)) {
					_pos = pos;
					return true;
				}
				return false;
			}
		};
		reg_base * _first;
		reg_base * _last;
		_position_type _begin;
		_position_type _end;
		_place_holder_t _place_holder;
		_position_type _result_begin;
		_position_type _result_end;
		bool _valid{false};
	public:
		reg_capture()
			: _first(0)
			, _last(0)
			, _place_holder(_end)
		{}
		void append(reg_base *reg) {
			if ( _first == 0 ) {
				_first = _last = reg;
			} else {
				_last->bind_next( reg );
				_last = reg;
			}
		}
		void insert(reg_base *reg, reg_base *with_reg) override {
			if (_first == reg) {
				_first = with_reg;
			}
			reg_base::insert(reg, with_reg);
		}
		bool match(_internal_reader_type &reader) override {
			_position_type begin = reader.pos();
			if (_first->match(reader)) {
				_result_begin = _begin = begin;
				_result_end = _end;
				_valid = true;
				return true;
			}
			return false;
		}
		bool match_no_fallback(_internal_reader_type &reader) override {
			_position_type begin = reader.pos();
			if (_first->match_no_fallback(reader)) {
				_result_begin = _begin = begin;
				_result_end = _end;
				_valid = true;
				return true;
			}
			return false;
		}
		bool match_no_fallback(_internal_reader_type &reader, reg_base *ext_next) {
			_position_type begin = reader.pos();
			if (_first->match_no_fallback(reader, ext_next)) {
				_result_begin = _begin = begin;
				_result_end = _end;
				_valid = true;
				return true;
			}
			return false;
		}
		bool search(_search_context &ctx, _internal_reader_type &reader) override {
			if (_first->search(ctx, reader)) {
				_result_begin = _begin = ctx.match_begin;
				_result_end = _end;
				_valid = true;
				return true;
			}
			return false;
		}
		void bind_next(reg_base *next) override {
			_last->bind_next(next);
			reg_base::bind_next(next);
		}
		const _position_type &begin() const override {
			return _result_begin;
		}
		const _position_type &end() const override {
			return _result_end;
		}
		void reset() override {
			_valid = false;
		}
		bool valid() const override {
			return _valid;
		}
		void complete() {
			append(&_place_holder);
		}
		void do_opt(simple_regex_t *rgx) override {
			_first->do_opt(rgx);
		}
	};

	class reg_or_exp : public reg_base {
		std::vector<reg_base*> _regs;

		struct _jumping_map_t {
			std::vector<reg_base*> map[128];
			const std::vector<reg_base*> &find(uint16_t value) {
				assert(value < 128);
				return map[value];
			}
		};

		_jumping_map_t *_jumping{nullptr};

		bool _jumping_match(_internal_reader_type &reader) {
			auto value = reader.peek();
			if (value >= 128) {
				return false;
			}
			auto &regs = _jumping->find(value);
			if (regs.empty()) {
				return false;
			}
			for (auto reg:regs) {
				if (reg->match(reader)) {
					return true;
				}
			}
			return false;
		}

		bool _jumping_search(_search_context &ctx, _internal_reader_type &reader) {
			while (!reader.end()) {
				ctx.match_begin = reader.pos();
				uint16_t value = reader.peek();
				if (value >= 128) {
					goto __not_matched;
				}
				{
					auto &regs = _jumping->find(value);
					if (regs.empty()) {
						goto __not_matched;
					}

					for (auto reg:regs) {
						if (reg->match(reader)) {
							goto __matched;
						}
					}
				}
__not_matched:
				reader.skip();
				continue;
__matched:
				ctx.match_end = reader.pos();
				return true;
			}
			return false;
		}
	public:
		reg_or_exp(bool ignore_case = false) : reg_base (false, ignore_case) {}
		~reg_or_exp() {
			if (_jumping) {
				delete _jumping;
				_jumping = nullptr;
			}
		}

		bool match(_internal_reader_type &reader) override {
			if (_jumping) {
				return _jumping_match(reader);
			}
			for (auto reg:_regs) {
				if (reg->match(reader)) {
					return true;
				}
			}
			return false;
		}

		bool match_no_fallback(_internal_reader_type &reader) override {
			if (_jumping) {
				return _jumping_match(reader);
			}
			for (auto reg:_regs) {
				if (reg->match(reader)) {
					return true;
				}
			}
			return false;
		}

		bool search(_search_context &ctx, _internal_reader_type &reader) override {
			if (_jumping) {
				return _jumping_search(ctx, reader);
			}
			return reg_base::search(ctx, reader);
		}

		void append(reg_base * sub_reg) {
			_regs.push_back( sub_reg );
		}

		void do_opt(simple_regex_t *rgx) override {
			// expand group
			for (auto iter = _regs.begin(), end = _regs.end(); iter != end;) {
				auto rgx = (*iter);
				if (rgx->type() == reg_group::ID) {
					*iter = static_cast<reg_group*>(rgx)->first();
					continue;
				}
				++iter;
			}
			// extract common parts
			auto front = _regs.front();
			for (auto reg:_regs) {
				if (!front->is_same(reg)) {
					goto __no_common_part;
				}
			}
			// remove common part
			for (auto iter = _regs.begin(), end = _regs.end(); iter != end;) {
				auto rgx = (*iter);
				auto next = rgx->get_next();
				if (next == this->_next) {
					_regs.erase(iter++);
					continue;
				}
				(*iter) = next;
				++iter;
			}

			for (auto reg:rgx->_reg_instances) {
				reg->insert(this, front);
			}
			if (rgx->_regex == this) {
				rgx->_regex = front;
			}

			front->do_opt(rgx);
			front->bind_next(this);

__no_common_part:
			for (auto reg:_regs) {
				reg->do_opt(rgx);
			}
			{
				std::vector<std::pair<uint16_t, reg_base *> > jump_map;
				size_t max_str_len = 0;
				uint16_t max_value = 0;
				for (auto reg:_regs) {
					if (reg->type() != reg_string::ID) {
						goto __do_next_opt;
					}
					auto str = static_cast<reg_string*>(reg)->get_str();
					if (str.empty()) {
						goto __do_next_opt;
					}
					uint16_t value = str[0];
					jump_map.emplace_back(value, reg);
					max_value = std::max<uint16_t>(value, max_value);
					max_str_len = std::max<size_t>(max_str_len, str.size());
				}
				if (max_str_len == 1) {
					// all the items are 1-char reg_string
					auto char_set = new reg_char_set_t<false>(_ignore_case);
					for(auto val:jump_map) {
						char_set->append(val.first);
					}
					// replace the or logic with reg_char_set
					auto new_reg = char_set->shink();
					for (auto reg:rgx->_reg_instances) {
						reg->insert(this, new_reg);
					}
					if (rgx->_regex == this) {
						rgx->_regex = new_reg;
					}
					rgx->_reg_instances.emplace_back(new_reg);

					auto this_next = this->get_next();
					for (auto iter = _regs.begin(); iter != _regs.end();) {
						auto next = (*iter)->get_next();
						if (next == nullptr || next == this_next) {
							iter = _regs.erase(iter);
						} else {
							(*(iter++)) = next;
						}
					}
					if (_regs.empty()) {
						new_reg->bind_next(this_next);
					} else {
						new_reg->bind_next(this);
					}
				} else if (max_value < 128) {
					// save the jumping map
					_jumping = new _jumping_map_t();
					for (auto item = jump_map.begin(); item != jump_map.end(); ++item) {
						_jumping->map[item->first].emplace_back(item->second);
					}
					this->is_fast_search = true;
				}
			}
__do_next_opt:
			auto next = this->get_next();
			if (next) {
				for (auto reg:_regs) {
					auto cur_reg = reg;
					auto next_reg = cur_reg->get_next();
					while(next_reg) {
						cur_reg = next_reg;
						next_reg = cur_reg->get_next();
					}
					cur_reg->bind_next(next);
				}
				next->do_opt(rgx);
			}
		}
	};

	class reg_line_begin : public reg_base {
	public:
		bool match(_internal_reader_type &reader) override {
			if (reader.is_line_head()) {
				return this->match_next(reader);
			}
			return false;
		}
		bool match_no_fallback(_internal_reader_type &reader) override {
			if (reader.is_line_head()) {
				return this->match_next_no_fallback(reader);
			}
			return false;
		}
	};

	class reg_line_end : public reg_base {
	public:
		bool match(_internal_reader_type &reader) override {
			if (reader.is_line_end()) {
				return this->match_next(reader);
			}
			return false;
		}
		bool match_no_fallback(_internal_reader_type &reader) override {
			if (reader.is_line_end()) {
				return this->match_next_no_fallback( reader );
			}
			return false;
		}
	};

	template <bool _IsBoundry = true>
	class reg_word_boundry : public reg_base {
	public:
		bool match(_internal_reader_type &reader) override {
			if (reader.is_word_boundry() == _IsBoundry) {
				return this->match_next(reader);
			}
			return false;
		}
		bool match_no_fallback(_internal_reader_type &reader) override {
			if (reader.is_word_boundry() == _IsBoundry) {
				return this->match_next_no_fallback(reader);
			}
			return false;
		}
	};

	// member
	std::list<reg_base*> _reg_instances;

	std::vector<capture_t*> _captures;

	static int32_t _hex_val( int32_t c ) {
		if (c>='0' && c<='9') {
			return c - '0';
		}
		if (c>='a' && c<='f') {
			return c - 'a';
		}
		if (c>='A' && c<='F') {
			return c - 'A';
		}
		return -1;
	}

	reg_base *_fix_limit(const char_type *&reg_str, reg_base *regex, bool no_capture) {
		reg_base *limit = 0;
		switch (*reg_str) {
		case '*':
			{
				reg_str++;
				limit = regex->set_limit(0);
				if (!limit) {
					limit = new reg_limit<false>(_ignore_case, regex, 0);
				}
				_reg_instances.push_back(limit);
				regex = limit;
			}
			break;
		case '+':
			{
				reg_str++;
				limit = regex->set_limit(1);
				if (!limit) {
					limit = new reg_limit<false>(_ignore_case, regex, 1);
				}
				_reg_instances.push_back(limit);
				regex = limit;
			}
			break;
		case '?':
			{
				reg_str++;
				limit = regex->set_limit(0, 1);
				if (!limit) {
					limit = new reg_limit<false>(_ignore_case, regex, 0, 1);
				}
				_reg_instances.push_back(limit);
				regex = limit;
			}
			break;
		case '{':
			{
				reg_str++;
				uint32_t min_limit = 0xFFFFFFFF;
				uint32_t max_limit = 0xFFFFFFFF;
				while (reg_str[0] >= '0' && reg_str[0] <= '9') {
					if (min_limit == 0xFFFFFFFF) {
						min_limit = reg_str[0]-'0';
					} else {
						min_limit = min_limit*10+(reg_str[0]-'0');
					}
					++reg_str;
				}
				if (reg_str[0] == ',') {
					++reg_str;
					while (reg_str[0] >= '0' && reg_str[0] <= '9') {
						if ( max_limit == 0xFFFFFFFF ) {
							max_limit = reg_str[0]-'0';
						} else {
							max_limit = max_limit*10+(reg_str[0]-'0');
						}
						++reg_str;
					}
					if ( min_limit == 0xFFFFFFFF ) {
						min_limit = 0;
					}
				} else {
					if ( min_limit == 0xFFFFFFFF ) {
						min_limit = 0;
					}
					max_limit = min_limit;
				}
				limit = regex->set_limit(min_limit, max_limit);
				if (!limit) {
					limit = new reg_limit<false>(_ignore_case, regex, min_limit, max_limit);
				}
				_reg_instances.push_back(limit);
				regex = limit;
				if (reg_str[0] != '}') {
					return 0;
				}
				++reg_str;
			}
		}
		if (limit && reg_str[0] == '?') {
			reg_str++;
			limit->set_min_match(true);
		}
		return regex;
	}

	reg_base *_fix_or(const char_type *&reg_str, reg_base *regex, bool no_capture) {
		reg_or_exp * or_exp = 0;
		reg_group * group = 0;
		while (*reg_str != 0 && *reg_str != ')') {
			if (*reg_str != '|') {
				if ( group == 0 ) {
					group = new reg_group();
					_reg_instances.push_back(group);
					group->append(regex);
					regex = group;
				}
				reg_base * sub_reg = _get_reg(reg_str, no_capture, false);
				if ( sub_reg ) {
					group->append(sub_reg);
				} else if ( or_exp ) {
					or_exp->append(group);
					break;
				}
			} else {
				reg_str++;
				if ( or_exp == 0 ) {
					or_exp = new reg_or_exp(_ignore_case);
					_reg_instances.push_back(or_exp);
				}
				or_exp->append(regex);
				group = 0;
				regex = _get_reg(reg_str, no_capture, false);
			}
		}
		if (or_exp) {
			if (regex) {
				or_exp->append(regex);
			}
			return or_exp;
		}
		return regex;
	}

	static reg_char_set *_make_char_set(bool inverse, bool ignore_case = false) {
		if (inverse) {
			return new reg_char_set_t<true>(ignore_case);
		}
		return new reg_char_set_t<false>(ignore_case);
	}

	static reg_base *_make_simple_char_set(bool inverse, uint16_t begin, uint16_t end, bool ignore_case = false) {
		if (inverse) {
			return new reg_simple_range<true>(begin, end, ignore_case);
		}
		return new reg_simple_range<false>(begin, end, ignore_case);
	}

	reg_base *_get_reg(const char_type *&reg_str, bool no_capture, bool fix_or_exp = true) {
		reg_base * result = 0;
		while (true) {
			uint16_t c = reg_str[0];
			reg_str++;
			switch (c) {
			case ')':
			case ']':
			case '*':
			case '+':
			case '?':
			case '|':
			case '}':
			case 0:
				return 0;
			case '^':
				result = new reg_line_begin();
				_reg_instances.push_back(result);
				goto _get_end;
			case '$':
				result = new reg_line_end();
				_reg_instances.push_back(result);
				goto _get_end;
			case '.':
				if (_single_line)
					result = new reg_any_single_line();
				else
					result = new reg_any();
				_reg_instances.push_back(result);
				goto _get_end;
			case '(': {
					reg_group * regex = 0;
					c = reg_str[0];
					if (c == '?') {
						if (reg_str[1] == ':') {
							reg_str += 2;
__no_capture:
							while (c != ')') {
								reg_base *sub_reg = _get_reg(reg_str, no_capture);
								if (sub_reg == 0) {
									return 0;
								}
								if (reg_str[0] == ')') {
									reg_str++;
									result = sub_reg;
									goto _get_end;
								}
								if (regex == 0) {
									regex = new reg_group();
									_reg_instances.push_back( regex );
								}
								regex->append(sub_reg);
								c = reg_str[0];
							}
							reg_str++;
							if (regex) {
								result = regex;
								goto _get_end;
							}
							break;
						}
					}
					{
						c = reg_str[0];
						if (no_capture) {
							goto __no_capture;
						}
						// capture by default
						reg_capture * regex = 0;
						while (c != ')') {
							reg_base *sub_reg = _get_reg(reg_str, no_capture);
							if (sub_reg == 0) {
								return 0;
							}
							if (regex == 0) {
								regex = new reg_capture();
								_reg_instances.push_back(regex);
								_captures.push_back(regex);
							}
							regex->append(sub_reg);
							c = reg_str[0];
						}
						reg_str++;
						if (regex) {
							regex->complete();
							result = regex;
							goto _get_end;
						}
					}
				}
			case '[':
				{
					bool inverse = false;
					if (reg_str[0] == '^') {
						inverse = true;
						reg_str ++;
					}
					reg_char_set * char_set = 0;
					auto get_char_set = [&]() {
						if (char_set == 0) {
							char_set = _make_char_set(inverse, _ignore_case);
						}
						return char_set;
					};

					while((c = reg_str[0]) != 0 && c != ']') {
						reg_str++;
						if (c == '\\') {
							switch(c = reg_str[0]) {
							case 'a':
								c = '\a';
								break;
							case 'f':
								c = '\f';
								break;
							case 'v':
								c = '\v';
								break;
							case 'n':
								c = '\n';
								break;
							case 'r':
								c = '\r';
								break;
							case 't':
								c = '\t';
								break;
							case 's':
								{
									auto char_set = get_char_set();
									char_set->append(0x09, 0x0D);
									char_set->append(' ');
								}
								++reg_str;
								continue;
							case 'S':
								{
									auto char_set = get_char_set();
									for (uint32_t i=0; i<=0xFFFF; ++i) {
										if (!space_chars::match(i)) {
											char_set->append(i);
										}
									}
								}
								++reg_str;
								continue;
							case 'w':
								{
									auto char_set = get_char_set();
									char_set->append( '_' );
									char_set->append('A', 'Z');
									char_set->append('a', 'z');
									char_set->append('0', '9');
								}
								++reg_str;
								continue;
							case 'd':
								{
									auto char_set = get_char_set();
									char_set->append('0', '9');
								}
								++reg_str;
								continue;
							case 'W':
								{
									auto char_set = get_char_set();
									for (uint32_t i=0; i<=0xFFFF; ++i) {
										if (!word_chars::match(i)) {
											char_set->append(i);
										}
									}
								}
								++reg_str;
								continue;
							case 'D':
								{
									auto char_set = get_char_set();
									for( uint32_t i=0; i<=0xFFFF; ++i ) {
										if (!(i >= '0' && i <= '9')) {
											char_set->append(i);
										}
									}
								}
								++reg_str;
								continue;
							case 'x':
								{
									int32_t v1 = _hex_val(reg_str[1]);
									int32_t v2 = _hex_val(reg_str[2]);
									if (v1 < 0 || v2 < 0) {
										return 0;
									}
									c = ((v1&0xF)<<4)+(v2&0xF);
									reg_str += 2;
									break;
								}
							case 'u':
								{
									int32_t v1 = _hex_val(reg_str[1]);
									int32_t v2 = _hex_val(reg_str[2]);
									int32_t v3 = _hex_val(reg_str[3]);
									int32_t v4 = _hex_val(reg_str[4]);
									if (v1 < 0 || v2 < 0 || v3 < 0 || v4 < 0) {
										return 0;
									}
									c = ((v1&0xF)<<12)+((v2&0xF)<<8)+((v3&0xF)<<4)+((v4&0xF)<<0);
									reg_str += 4;
									break;
								}
							case '\\':
							case '|':
							case '[':
							case ']':
							case '(':
							case ')':
							case '{':
							case '}':
							case '.':
							case '*':
							case '+':
							case '-':
							case '?':
							case '$':
							case '^':
								break;
							case 0:
							default:
								return 0;
							}
							reg_str++;
						}
						if (reg_str[0] == '-') {
							reg_str++;
							wchar_t c2 = reg_str[0];
							if (c2 == '\\') {
								reg_str++;
								switch(c2 = reg_str[0]) {
								case 'a':
									c2 = '\a';
									break;
								case 'f':
									c2 = '\f';
									break;
								case 'v':
									c2 = '\v';
									break;
								case 'n':
									c2 = '\n';
									break;
								case 'r':
									c2 = '\r';
									break;
								case 't':
									c2 = '\t';
									break;
								case 'x':
									{
										int32_t v1 = _hex_val(reg_str[1]);
										int32_t v2 = _hex_val(reg_str[2]);
										if (v1 < 0 || v2 < 0) {
											return 0;
										}
										c2 = ((v1&0xF)<<4)+(v2&0xF);
										reg_str += 2;
										break;
									}
								case 'u':
									{
										int32_t v1 = _hex_val(reg_str[1]);
										int32_t v2 = _hex_val(reg_str[2]);
										int32_t v3 = _hex_val(reg_str[3]);
										int32_t v4 = _hex_val(reg_str[4]);
										if (v1 < 0 || v2 < 0 || v3 < 0 || v4 < 0) {
											return 0;
										}
										c2 = ((v1&0xF)<<12)+((v2&0xF)<<8)+((v3&0xF)<<4)+((v4&0xF)<<0);
										reg_str += 4;
										break;
									}
								case '\\':
								case '|':
								case '[':
								case ']':
								case '(':
								case ')':
								case '{':
								case '}':
								case '.':
								case '*':
								case '+':
								case '-':
								case '?':
								case '^':
								case '$':
									break;
								case 0:
								default:
									return 0;
								}
							}
							reg_str++;
							if (c <= c2) {
								if (char_set == nullptr && reg_str[0] == ']') {
									// simple range
									result = _make_simple_char_set(inverse, c, c2, _ignore_case);
									_reg_instances.push_back(result);
									c = ']';
									break;
								}
								auto char_set = get_char_set();
								char_set->append(c, c2);
							}
						} else {
							auto char_set = get_char_set();
							char_set->append(c);
						}
					}
					if (c == ']') {
						reg_str++;
					}
					if (char_set != 0) {
						result = char_set->shink();
						_reg_instances.push_back(result);
						goto _get_end;
					}
					if (result != nullptr) {
						goto _get_end;
					}
				}
				break;
			case '\\':
				{
					switch(reg_str[0]) {
					case 'D':
					case 'd':
						{
							bool inverse = (reg_str[0] == 'D');
							auto char_set = _make_simple_char_set(inverse, '0', '9');
							_reg_instances.push_back(char_set);
							++reg_str;
							result = char_set;
							goto _get_end;
						}
						break;
					case 'W':
					case 'w':
						{
							bool inverse = (reg_str[0] == 'W');
							reg_base *char_set;
							if (inverse) {
								char_set = new reg_word_chars<true>();
							} else {
								char_set = new reg_word_chars<false>();
							}
							_reg_instances.push_back(char_set);
							++reg_str;
							result = char_set;
							goto _get_end;
						}
						break;
					case 'S':
					case 's':
						{
							bool inverse = (reg_str[0] == 'S');
							reg_base *char_set;
							if (inverse) {
								char_set = new reg_space_chars<true>();
							} else {
								char_set = new reg_space_chars<false>();
							}
							_reg_instances.push_back(char_set);
							++reg_str;
							result = char_set;
							goto _get_end;
						}
						break;
					case 'b':
						{
							result = new reg_word_boundry<true>();
							_reg_instances.push_back(result);
							++reg_str;
							goto _get_end;
						}
						break;
					case 'B':
						{
							result = new reg_word_boundry<false>();
							_reg_instances.push_back(result);
							++reg_str;
							goto _get_end;
						}
						break;
					}
				}
			default:
				{
					reg_string * str = 0;
					const char_type *reg_str_head = reg_str-1;
					do {
						const char_type *reg_str_begin = reg_str-1;
						if (c == '\\') {
							switch(c = reg_str[0]) {
							case 'a':
								c = '\a';
								break;
							case 'f':
								c = '\f';
								break;
							case 'v':
								c = '\v';
								break;
							case 'n':
								c = '\n';
								break;
							case 'r':
								c = '\r';
								break;
							case 't':
								c = '\t';
								break;
							case 'x':
								{
									int32_t v1 = _hex_val(reg_str[1]);
									int32_t v2 = _hex_val(reg_str[2]);
									if ( v1 < 0 || v2 < 0 ) {
										return 0;
									}
									c = ((v1&0xF)<<4)+(v2&0xF);
									reg_str += 2;
									break;
								}
							case 'u':
								{
									int32_t v1 = _hex_val(reg_str[1]);
									int32_t v2 = _hex_val(reg_str[2]);
									int32_t v3 = _hex_val(reg_str[3]);
									int32_t v4 = _hex_val(reg_str[4]);
									if ( v1 < 0 || v2 < 0 || v3 < 0 || v4 < 0 ) {
										return 0;
									}
									c = ((v1&0xF)<<12)+((v2&0xF)<<8)+((v3&0xF)<<4)+((v4&0xF)<<0);
									reg_str += 4;
									break;
								}
							case '\\':
							case '|':
							case '[':
							case ']':
							case '(':
							case ')':
							case '.':
							case '*':
							case '+':
							case '?':
							case '{':
							case '}':
							case '^':
							case '$':
								break;
							case 'D':case 'd':
							case 'W':case 'w':
							case 'S':case 's':
							case 'b':
								{
									reg_str = reg_str_begin;
									goto __get_string_end;
								}
							case 0:
							default:
								return 0;
							}
							reg_str++;
						} else {
							switch(c) {
							case '[':
							case ']':
							case '(':
							case ')':
							case '+':
							case '*':
							case '?':
							case '.':
							case '|':
							case '{':
							case '^':
							case '$':
								reg_str = reg_str_begin;
								goto __get_string_end;
							}
						}
						if ( str == 0 ) {
							str = new reg_string(_ignore_case);
							_reg_instances.push_back(str);
						}
						str->append(c);
					} while ((c=reg_str[0]) && c && reg_str++);
__get_string_end:
					if (!str) {
						if (reg_str_head == reg_str) {
							// some thing wrong
							return 0;
						}
						break;
					}
					result = str;
					goto _get_end;
				}
				break;
			}
		}
		return 0;
_get_end:
		result = _fix_limit(reg_str, result, no_capture);
		if (fix_or_exp) {
			return _fix_or(reg_str, result, no_capture);
		} else {
			return result;
		}
	}
	reg_base * _regex;
	_position_type _match_begin;
	_position_type _match_end;
	uint32_t _cur_status;
	_internal_reader_type *_internal_reader{nullptr};
	void _clean_result() {
		_match_begin = _match_end = _position_type();
		for (size_t i=_captures.size(); i>0;) {
			_captures[--i]->reset();
		}
	}
	_internal_reader_type &_get_reader(_TReader &reader) {
		if (!_internal_reader) {
			_internal_reader = new _internal_reader_type(*this);
		}

		_internal_reader->rebind(&reader);
		return *_internal_reader;
	}
public:
	simple_regex_t(const char_type *reg, bool no_capture = false, int32_t flag = 0)
		: _cur_status(RGX_LINE_HEAD|RGX_WORD_BOUNDRY)
	{
		parse(reg, no_capture, flag);
	}
	simple_regex_t()
		: _regex(0)
		, _cur_status(RGX_LINE_HEAD|RGX_WORD_BOUNDRY)
	{}
	~simple_regex_t() {
		clear();
	}
	bool parse(const char_type *reg, bool no_capture = false, int32_t flag = 0) override {
		clear();
		_single_line = (flag & 0x1);
		_ignore_case = (flag & 0x2);
		_regex = _get_reg(reg, no_capture);
		if (_regex) {
			_regex->do_opt(this);
			reset();
			return true;
		}
		return false;
	}
	void clear() override {
		while( !_reg_instances.empty() ) {
			delete _reg_instances.back();
			_reg_instances.pop_back();
		}
		_captures.clear();
		if (_internal_reader) {
			delete _internal_reader;
			_internal_reader = nullptr;
		}
	}
	void reset(uint32_t status = RGX_LINE_HEAD|RGX_WORD_BOUNDRY) override {
		_cur_status = status;
		if (_internal_reader) {
			_internal_reader->reset_char();
		}
		_clean_result();
	}
	bool search(_TReader &reader) override {
		if (_regex == 0) {
			return false;
		}
		_clean_result();
		_internal_reader_type &_internal_reader = _get_reader(reader);
		_search_context ctx{_match_begin, _match_end};
		return _regex->search(ctx, _internal_reader);
	}
	bool match(_TReader &reader) override {
		_clean_result();
		_internal_reader_type &_internal_reader = _get_reader(reader);
		_match_begin = _internal_reader.pos();
		bool result = _regex->match(_internal_reader);
		_match_end = _internal_reader.pos();
		return result;
	}
	bool full_match(_TReader &reader) override {
		return match(reader) && reader.end();
	}
	bool search(_TReader &&reader) override {
		return search(reader);
	}
	bool match(_TReader &&reader) override {
		return match(reader);
	}
	bool full_match(_TReader &&reader) override {
		return match(reader) && reader.end();
	}
	size_t capture_count() const override {
		return _captures.size();
	}
	const capture_t &get_capture(size_t idx) override {
		return *_captures[idx];
	}
	bool valid() const override {
		return _regex != 0;
	}
	virtual const _position_type &begin() const override {
		return _match_begin;
	}
	virtual const _position_type &end() const override {
		return _match_end;
	}
	static bool is_word_boundry(uint16_t c, uint16_t c2) {
		return _internal_reader_type::test_word_boundry(c, c2);
	}

};


/*
using regex_fast_reader = regex_fast_reader_t<wchar_t>;
using regex_reader = regex_reader_t<wchar_t> ;
using simple_regex = simple_regex_t<regex_reader>;
using simple_regex_fast = simple_regex_t<regex_fast_reader>;
*/


namespace piv {
	namespace regex {
		/**
		 * @brief 匹配
		 * @param reg 正则表达式
		 * @param text 欲匹配内容
		 * @param text_len 内容长度
		 * @return 是否匹配
		 */
		template<typename CharT = wchar_t>
		bool Match(const CharT *reg, const CharT *text, size_t text_len = (size_t)-1, int32_t flag = 0) {
			simple_regex_t<regex_fast_reader_t<CharT>> rgx(reg, true, flag);
			if (rgx.valid())
				return rgx.match(text_len == (size_t)-1 ? regex_fast_reader_t<CharT>(text) : regex_fast_reader_t<CharT>(text, text_len));
			return false;
		}

		template<typename T>
		bool Match(const T &reg, const CVolString &text, int32_t flag = 0) {
			return Match<wchar_t>(PivFromAny<wchar_t>(reg), text.GetText(), text.GetLength(), flag);
		}

		template<typename CharT, typename T>
		bool Match(const T &reg, const CVolMem &text, int32_t flag = 0) {
			return Match<CharT>(PivFromAny<CharT>(reg), reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize() / sizeof(CharT)), flag);
		}

		template<typename CharT, typename T>
		bool Match(const T &reg, const piv::basic_string_view<CharT> &text, int32_t flag = 0) {
			return Match<CharT>(PivFromAny<CharT>(reg), text.data(), text.size(), flag);
		}
	
		/**
		 * @brief 完全匹配
		 * @param reg 正则表达式
		 * @param text 欲匹配内容
		 * @param text_len 内容长度
		 * @return 是否匹配
		 */
		template<typename CharT = wchar_t>
		bool FullMatch(const CharT *reg, const CharT *text, size_t text_len = -1, int32_t flag = 0) {
			simple_regex_t<regex_fast_reader_t<CharT>> rgx(reg, true, flag);
			if (rgx.valid())
				return rgx.full_match(text_len == (size_t)-1 ? regex_fast_reader_t<CharT>(text) : regex_fast_reader_t<CharT>(text, text_len));
			return false;
		}

		template<typename T>
		bool FullMatch(const T &reg, const CVolString &text, int32_t flag = 0) {
			return FullMatch<wchar_t>(PivFromAny<wchar_t>(reg), text.GetText(), text.GetLength(), flag);
		}

		template<typename CharT, typename T>
		bool FullMatch(const T &reg, const CVolMem &text, int32_t flag = 0) {
			return FullMatch<CharT>(PivFromAny<CharT>(reg), reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize() / sizeof(CharT)), flag);
		}

		template<typename CharT, typename T>
		bool FullMatch(const T &reg, const piv::basic_string_view<CharT> &text, int32_t flag = 0) {
			return FullMatch<CharT>(PivFromAny<CharT>(reg), text.data(), text.size(), flag);
		}

		/**
		 * @brief 正则替换
		 * @param text 欲匹配内容
		 * @param text_len 内容长度
		 * @param reg 正则表达式
		 * @param fmt 正则替换格式
		 * @param out 替换结果
		 * @param offset 开始位置
		 * @param max_times 替换次数
		 * @param flag 匹配模式
		 * @return 实际替换次数
		 */
		template<typename CharT>
		int Replace(const CharT *text, size_t text_len, const CharT *reg, const CharT *fmt, CVolString &out, size_t offset = 0, size_t max_times = (size_t)-1, int32_t flag = 0) {
			out.Empty();
			out.SetNumAlignChars(text_len * 4 / 3);
			simple_regex_t<regex_fast_reader_t<CharT>> rgx(reg, false, flag);
			if (!rgx.valid())
				return 0;
			regex_fast_reader_t<CharT> reader(text, text_len);
			if (offset > 0)
				reader.set_pos(text + (offset > text_len ? text_len : offset));
			int times = 0;
			const CharT * _off = text;
			while (times < max_times && rgx.search(reader) == true) {
				if (rgx.begin() - _off > 0)
					out.AddText(_off, rgx.begin() - _off);
				_off = rgx.end();
				const CharT* c = &fmt[0];
				while (*c != 0) {
					if (*c == '\\') {
						CharT n = *(++c);
						if (n >= '0' && n <= '9') {
							n -= '0';
							if (n == 0) {
								out.AddText(rgx.begin(), static_cast<INT_P>(rgx.end() - rgx.begin()));
							} else if (n <= rgx.capture_count()) {
								auto &cap = rgx.get_capture(n - 1);
								if (cap.valid())
									out.AddText(cap.begin(), static_cast<INT_P>(cap.end() - cap.begin()));
							}
						} else {
							out.AddChar('\\');
							out.AddChar(n);
						}
					} else {
						out.AddChar(*c);
					}
					c++;
				}
				times++;
			}
			if (_off < reader.end_pos())
				out.AddText(_off, reader.end_pos() - _off);
			return times;
		}

		template<typename CharT>
		int Replace(const CharT *text, size_t text_len, const CharT *reg, const CharT *fmt, std::basic_string<CharT> &out, size_t offset = 0, size_t max_times = (size_t)-1, int32_t flag = 0) {
			out.clear();
			out.reserve(text_len * 4 / 3);
			simple_regex_t<regex_fast_reader_t<CharT>> rgx(reg, false, flag);
			if (!rgx.valid())
				return 0;
			regex_fast_reader_t<CharT> reader(text, text_len);
			if (offset > 0)
				reader.set_pos(text + (offset > text_len ? text_len : offset));
			int times = 0;
			const CharT * _off = text;
			while (times < max_times && rgx.search(reader) == true) {
				if (rgx.begin() - _off > 0)
					out.append(_off, rgx.begin());
				_off = rgx.end();
				const CharT* c = &fmt[0];
				while (*c != 0) {
					if (*c == '\\') {
						CharT n = *(++c);
						if (n >= '0' && n <= '9') {
							n -= '0';
							if (n == 0) {
								out.append(rgx.begin(), rgx.end());
							} else if (n <= rgx.capture_count()) {
								auto &cap = rgx.get_capture(n - 1);
								if (cap.valid())
									out.append(cap.begin(), cap.end());
							}
						} else {
							out.push_back('\\');
							out.push_back(n);
						}
					} else {
						out.push_back(*c);
					}
					c++;
				}
				times++;
			}
			if (_off < reader.end_pos())
				out.append(_off, reader.end_pos());
			return times;
		}

		template<typename CharT>
		int Replace(const CharT *text, size_t text_len, const CharT *reg, const CharT *fmt, CVolMem &out, size_t offset = 0, size_t max_times = (size_t)-1, int32_t flag = 0) {
			out.Empty();
			out.SetMemAlignSize(text_len * sizeof(CharT) * 4 / 3);
			simple_regex_t<regex_fast_reader_t<CharT>> rgx(reg, false, flag);
			if (!rgx.valid())
				return 0;
			regex_fast_reader_t<CharT> reader(text, text_len);
			if (offset > 0)
				reader.set_pos(text + (offset > text_len ? text_len : offset));
			int times = 0;
			const CharT * _off = text;
			while (times < max_times && rgx.search(reader) == true) {
				if (rgx.begin() - _off > 0)
					out.Append(_off, static_cast<INT_P>(rgx.begin() - _off) * sizeof(CharT));
				_off = rgx.end();
				const CharT* c = &fmt[0];
				while (*c != 0) {
					if (*c == '\\') {
						CharT n = *(++c);
						if (n >= '0' && n <= '9') {
							n -= '0';
							if (n == 0) {
								out.Append(rgx.begin(), static_cast<INT_P>(rgx.end() - rgx.begin()) * sizeof(CharT));
							} else if (n <= rgx.capture_count()) {
								auto &cap = rgx.get_capture(n - 1);
								if (cap.valid())
									out.Append(cap.begin(), static_cast<INT_P>(cap.end() - cap.begin()) * sizeof(CharT));
							}
						} else {
							if (sizeof(CharT) == 2) {
								out.AddWChar('\\');
								out.AddWChar(n);
							} else {
								out.AddU8Char('\\');
								out.AddU8Char(n);
							}
						}
					} else {
						if (sizeof(CharT) == 2)
							out.AddWChar(*c);
						else
							out.AddU8Char(*c);
					}
					c++;
				}
				times++;
			}
			if (_off < reader.end_pos())
				out.Append(_off, static_cast<INT_P>(reader.end_pos() - _off) * sizeof(CharT));
			return times;
		}

		template<typename CharT, typename R, typename F, typename T>
		int Replace(const CVolString &text, const R &reg, const F &fmt, T &out, size_t offset = 0, size_t max_times = (size_t)-1, int32_t flag = 0) {
			return Replace<CharT>(text.GetText(), text.GetLength(), PivFromAny<CharT>(reg), PivFromAny<CharT>(fmt), out, offset, max_times, flag);
		}

		template<typename CharT, typename R, typename F, typename T>
		int Replace(const std::basic_string<CharT> &text, const R &reg, const F &fmt, T &out, size_t offset = 0, size_t max_times = (size_t)-1, int32_t flag = 0) {
			return Replace<CharT>(text.c_str(), text.size(), PivFromAny<CharT>(reg), PivFromAny<CharT>(fmt), out, offset, max_times, flag);
		}

		template<typename CharT, typename R, typename F, typename T>
		int Replace(const piv::basic_string_view<CharT> &text, const R &reg, const F &fmt, T &out, size_t offset = 0, size_t max_times = (size_t)-1, int32_t flag = 0) {
			return Replace<CharT>(text.data(), text.size(), PivFromAny<CharT>(reg), PivFromAny<CharT>(fmt), out, offset, max_times, flag);
		}

		template<typename CharT, typename R, typename F, typename T>
		int Replace(const CVolMem &text, const R &reg, const F &fmt, T &out, size_t offset = 0, size_t max_times = (size_t)-1, int32_t flag = 0) {
			return Replace<CharT>(reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize() / sizeof(CharT)), PivFromAny<CharT>(reg), PivFromAny<CharT>(fmt), out, offset, max_times, flag);
		}

	} // namespace regex
} // namespace piv


template<typename CharT = wchar_t>
class PivRegex {
private:
	simple_regex_t<regex_fast_reader_t<CharT>> m_rgx;
	std::unique_ptr<regex_fast_reader_t<CharT>> m_reader;
	std::unique_ptr<std::basic_string<CharT>> m_buffer;
	std::unique_ptr<std::basic_string<CharT>> m_out;
	const CharT * m_off = nullptr;

public:
	PivRegex(){}
	~PivRegex() {
		Clear(true);
	}

	/**
	 * @brief 清空
	 */
	void Clear(bool del_replaced = true) {
		m_rgx.clear();
		m_reader.reset(nullptr);
		m_buffer.reset(nullptr);
		m_off = nullptr;
		if (del_replaced)
			m_out.reset(nullptr);
	}

	/**
	 * @brief 查找
	 * @param text 欲查找内容
	 * @param text_len 内容长度
	 * @param reg 正则表达式
	 * @param isCache 是否缓存文本
	 * @return 是否匹配
	 */
	bool Search(const CharT *text, size_t text_len, const CharT *reg, int32_t flag = 0) {
		Clear(true);
		if (m_rgx.parse(reg, false, flag)) {
			if (flag & 0x4) {
				m_buffer.reset(new std::basic_string<CharT>(text, text_len));
				m_reader.reset(new regex_fast_reader_t<CharT>(m_buffer->c_str(), m_buffer->size()));
			} else {
				m_reader.reset(new regex_fast_reader_t<CharT>(text, text_len));
			}
			return m_rgx.search(*m_reader);
		}
		return false;
	}

	template<typename T>
	bool Search(const CVolString &text, const T &reg, int32_t flag = 0) {
		return Search(text.GetText(), static_cast<size_t>(text.GetLength()), PivFromAny<CharT>(reg), flag);
	}

	template<typename T>
	bool Search(const std::basic_string<CharT> &text, const T &reg, int32_t flag = 0) {
		return Search(text.c_str(), text.size(), PivFromAny<CharT>(reg), flag);
	}

	template<typename T>
	bool Search(const piv::basic_string_view<CharT> &text, const T &reg, int32_t flag = 0) {
		return Search(text.data(), text.size(), PivFromAny<CharT>(reg), flag);
	}

	template<typename T>
	bool Search(const CVolMem &text, const T &reg, int32_t flag = 0) {
		return Search(reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize() / sizeof(CharT)), PivFromAny<CharT>(reg), flag);
	}

	/**
	 * @brief 查找下一个
	 * @return 是否匹配
	 */
	inline bool SearchNext() {
		if (m_rgx.valid() && m_reader)
			return m_rgx.search(*m_reader);
		return false;
	}

	/**
	 * @brief 取匹配文本
	 * @tparam R 返回文本类型
	 * @param idx 索引
	 * @return 匹配文本
	 */
	template<typename R>
	inline R GetMatchText() {
		if (m_rgx.valid() && m_rgx.begin() && m_rgx.end())
			return R(m_rgx.begin(), m_rgx.end() - m_rgx.begin());
		return R();
	}

	/**
	 * @brief 取子匹配数量
	 * @return 子匹配数量
	 */
	inline size_t GetGroupCount() {
		return m_rgx.capture_count();
	}

	/**
	 * @brief 取子匹配文本
	 * @tparam R 返回文本类型
	 * @param idx 索引
	 * @return 子匹配文本
	 */
	template<typename R>
	R GetGrounpText(size_t idx) {
		if (idx == 0) {
			if (m_rgx.valid() && m_rgx.begin() && m_rgx.end())
				return R(m_rgx.begin(), m_rgx.end() - m_rgx.begin());
		} else if (idx <= m_rgx.capture_count()) {
			auto& cap = m_rgx.get_capture(idx - 1);
			if (cap.valid())
				return R(cap.begin(), cap.end() - cap.begin());
		}
		return R();
	}

	/**
	 * @brief 是否存在匹配
	 * @return 
	 */
	inline bool IsMatch() {
		return (m_rgx.valid() && m_reader) ? (m_rgx.begin() != nullptr && m_rgx.end() != nullptr) : false;
	}

	/**
	 * @brief 替换
	 * @param fmt 正则替换格式
	 * @return 是否成功
	 */
	template<typename T>
	bool Replace(const T &fmt) {
		if (!IsMatch())
			return false;
		if (!m_out || !m_off) {
			m_out.reset(new std::basic_string<CharT>{});
			m_out->reserve(m_reader->size() * 4 / 3);
			m_off = m_reader->data();
		}
		if (m_rgx.begin() - m_off > 0)
			m_out->append(m_off, m_rgx.begin());
		m_off = m_rgx.end();
		PivFromAny<CharT> _fmt{fmt};
		const CharT* c = &_fmt.GetText()[0];
		while (*c != 0) {
			if (*c == '\\') {
				CharT n = *(++c);
				if (n >= '0' && n <= '9') {
					n -= '0';
					if (n == 0) {
						m_out->append(m_rgx.begin(), m_rgx.end());
					} else if (n <= m_rgx.capture_count()) {
						auto &cap = m_rgx.get_capture(n - 1);
						if (cap.valid())
							m_out->append(cap.begin(), cap.end());
					}
				} else {
					m_out->push_back('\\');
					m_out->push_back(n);
				}
			} else {
				m_out->push_back(*c);
			}
			c++;
		}
		return true;
	}

	/**
	 * @brief 取替换文本
	 * @return 
	 */
	std::basic_string<CharT> &GetReplacedText() {
		if (!m_out) {
			m_off = nullptr;
			if (m_reader)
				m_out.reset(new std::basic_string<CharT>{m_reader->data(), m_reader->size()});
			else
				m_out.reset(new std::basic_string<CharT>{});
		} else {
			if (m_reader) {
				if (m_off < m_reader->end_pos())
					m_out->append(m_off, m_reader->end_pos());
			}
		}
		Clear(false);
		return *m_out;
	}

}; // class PivRegex

#undef __SIMPLE_REGEX_LIKELY__
#undef __SIMPLE_REGEX_UNLIKELY__


#endif // _PIV_SIMPLE_REGEX_H
