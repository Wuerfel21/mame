// license:BSD-3-Clause
// copyright-holders:Couriersud

///
/// \file test_pmfp_multibase.cpp
///
/// tests for plib::pmfp
///

#include "plib/ptypes.h"
#include "plib/pexception.h"
#include "plib/ppmf.h"

#include <utility>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <iostream>

template <class F, class S>
typename std::enable_if_t<!plib::has_ostream_operator<std::basic_ostream<char>, std::pair<F, S>>::value, std::basic_ostream<char>&>
operator << (std::basic_ostream<char>& os, const std::pair<F, S> &p)
{
	os << "{ " << p.first << ", " << p.second << " }";
	return os;
}

#include "plib/ptests.h"

template <class T>
typename std::enable_if_t<!plib::has_ostream_operator<std::basic_ostream<char>, T>::value, std::basic_ostream<char>&>
operator << (std::basic_ostream<char>& os, const T &p)
{
	plib::unused_var(p);
	os << std::string(typeid(T).name());
	return os;
}

class pmfp_test_complex_return : public plib::testing::Test
{
protected:
	using ret_t = std::pair<void *, int>;

	//using test_delegate1 = plib::pmfp<ret_t (int, int)>;
	using test_delegate = plib::pmfp<ret_t ()>;

	class a
	{
	public:
		ret_t ap() { return ret_t(static_cast<void *>(this), ax); }
		int ax;
	};

	class b
	{
	public:
		ret_t bp() { return ret_t(static_cast<void *>(this), bx); }
		int bx;
	};

	class multibase : public a, public b
	{
	public:
	};


	class vtb
	{
	public:
		virtual ~vtb() = default;
		virtual ret_t vp1() { return ret_t(static_cast<void *>(this), 1); }
		virtual ret_t vp2() { return ret_t(static_cast<void *>(this), 2); }
	};

	class vti1 : public vtb
	{
	public:
		virtual ret_t vp1() override { return ret_t(static_cast<void *>(this), 3); }
		virtual ret_t vp2() override { return ret_t(static_cast<void *>(this), 4); }
	};

	class vti2 : public a, public vtb
	{
	public:
		virtual ret_t vp1() override { return ret_t(static_cast<void *>(this), 13); }
		virtual ret_t vp2() override { return ret_t(static_cast<void *>(this), 14); }
	};

	class forward;

	test_delegate make_forward_delegate(ret_t (forward::*func)(), forward *obj)
	{
		return test_delegate(func, obj);
	}

	class vb
	{
	public:
		ret_t xp() { return ret_t(static_cast<void *>(this), x); }
		int x=10;
	};

	class vd1 : public virtual vb
	{
	public:
		ret_t yp() { return ret_t(static_cast<void *>(this), y); }
		int y;
	};

	class vd2 : public virtual vb
	{
	public:
		ret_t zp() { return ret_t(static_cast<void *>(this), z); }
		int z;
	};

	class forward : public vd1, public vd2
	{
	public:
	};

};

class pmfp_test_simple_return : public plib::testing::Test
{
protected:
	using ret_t = std::pair<void *, int>;

	//using test_delegate1 = plib::pmfp<ret_t (int, int)>;
	using test_delegate = plib::pmfp<void (ret_t &)>;

	class a
	{
	public:
		void ap(ret_t &r) { r = ret_t(static_cast<void *>(this), ax); }
		int ax;
	};

	class b
	{
	public:
		void bp(ret_t &r) { r = ret_t(static_cast<void *>(this), bx); }
		int bx;
	};

	class multibase : public a, public b
	{
	public:
	};


	class vtb
	{
	public:
		virtual ~vtb() = default;
		virtual void vp1(ret_t &r) { r = ret_t(static_cast<void *>(this), 1); }
		virtual void vp2(ret_t &r) { r = ret_t(static_cast<void *>(this), 2); }
	};

	class vti1 : public vtb
	{
	public:
		virtual void vp1(ret_t &r) override { r = ret_t(static_cast<void *>(this), 3); }
		virtual void vp2(ret_t &r) override { r = ret_t(static_cast<void *>(this), 4); }
	};

	class vti2 : public a, public vtb
	{
	public:
		virtual void vp1(ret_t &r) override { r = ret_t(static_cast<void *>(this), 13); }
		virtual void vp2(ret_t &r) override { r = ret_t(static_cast<void *>(this), 14); }
	};

	class forward;

	test_delegate make_forward_delegate(void (forward::*func)(ret_t &), forward *obj)
	{
		return test_delegate(func, obj);
	}

	class vb
	{
	public:
		void xp(ret_t &r) {  r = ret_t(static_cast<void *>(this), x); }
		int x=10;
	};

	class vd1 : public virtual vb
	{
	public:
		void yp(ret_t &r) {  r = ret_t(static_cast<void *>(this), y); }
		int y;
	};

	class vd2 : public virtual vb
	{
	public:
		void zp(ret_t &r) { r = ret_t(static_cast<void *>(this), z); }
		int z;
	};

	class forward : public vd1, public vd2
	{
	public:
	};
};

PTEST_F(pmfp_test_complex_return, multibase_test)
{
	multibase obj;
	obj.ax = 1;
	obj.bx = 2;
	test_delegate f(&multibase::ap, &obj);
	test_delegate g(&multibase::bp, &obj);

	// a=0x63fbf8
	// b=0x63fbfc
	// a=0x63fbf8(1)
	// b=0x63fbfc(2)
	// b=0x63fbfc(2)
	// b=0x63fbfc(2)
	PEXPECT_EQ(f(), ret_t(static_cast<void *>(static_cast<a *>(&obj)), 1));
	auto g_ret = g();
	PEXPECT_EQ(g_ret, ret_t(static_cast<void *>(static_cast<b *>(&obj)), 2));

	test_delegate h = g;
	PEXPECT_EQ(g_ret, h());
	f = g;
	PEXPECT_EQ(g_ret, f());

	//vti1=0xdf6fb0.vp1
	//vti1=0xdf6fb0.vp2

	std::unique_ptr<vtb> ptr;
	ptr.reset(new vti1);
	f = test_delegate(&vtb::vp1, ptr.get());
	auto f1_ret = f();
	f = test_delegate(&vtb::vp2, ptr.get());
	auto f2_ret = f();
	PEXPECT_EQ(f1_ret.first, f2_ret.first);
	PEXPECT_EQ(f1_ret.second, 3);
	PEXPECT_EQ(f2_ret.second, 4);

	// vti2=0xdf6d50.vp1
	// vti2=0xdf6d50.vp2
	ptr.reset(new vti2);
	f = test_delegate(&vtb::vp1, ptr.get());
	f1_ret = f();
	f = test_delegate(&vtb::vp2, ptr.get());
	f2_ret = f();
	PEXPECT_EQ(f1_ret.first, f2_ret.first);
	PEXPECT_EQ(f1_ret.second, 13);
	PEXPECT_EQ(f2_ret.second, 14);

	//vb=0x63fafc
	//vd1=0x63fae0
	//vd2=0x63faf0
	//vd1=0x63fae0(8)
	//vd2=0x63faf0(9)

	forward obj2;
	obj2.x = 7;
	obj2.y = 8;
	obj2.z = 9;
	PEXPECT_NE(static_cast<void *>(static_cast<vb *>(&obj2)), static_cast<void *>(static_cast<vd1 *>(&obj2)));
	PEXPECT_NE(static_cast<void *>(static_cast<vb *>(&obj2)), static_cast<void *>(static_cast<vd2 *>(&obj2)));

#if defined(_MSC_VER) && !defined(__clang__)
	f = make_forward_delegate(&forward::xp, &obj2);
	PEXPECT_TRUE(f().second == 7);
	std::cout << f().second << " " << obj2.xp().second << "\n";
#endif
	f = make_forward_delegate(&forward::yp, &obj2);
	PEXPECT_EQ(f().second, 8);
	f = make_forward_delegate(&forward::zp, &obj2);
	PEXPECT_EQ(f().second, 9);
}

PTEST_F(pmfp_test_simple_return, multibase_test)
{
	multibase obj;
	obj.ax = 1;
	obj.bx = 2;
	test_delegate f(&multibase::ap, &obj);
	test_delegate g(&multibase::bp, &obj);

	// a=0x63fbf8
	// b=0x63fbfc
	// a=0x63fbf8(1)
	// b=0x63fbfc(2)
	// b=0x63fbfc(2)
	// b=0x63fbfc(2)
	ret_t fr;
	f(fr);
	PEXPECT_EQ(fr, ret_t(static_cast<void *>(static_cast<a *>(&obj)), 1));
	ret_t gr;
	g(gr);
	PEXPECT_EQ(gr, ret_t(static_cast<void *>(static_cast<b *>(&obj)), 2));
	//PEXPECT_EQ(f, g);

	test_delegate h = g;
	ret_t hr;
	h(hr);
	PEXPECT_EQ(gr, hr);
	f = g;
	f(fr);
	PEXPECT_EQ(gr, fr);

	//vti1=0xdf6fb0.vp1
	//vti1=0xdf6fb0.vp2

	std::unique_ptr<vtb> ptr;
	ptr.reset(new vti1);
	f = test_delegate(&vtb::vp1, ptr.get());
	ret_t f1_ret;
	f(f1_ret);
	f = test_delegate(&vtb::vp2, ptr.get());
	ret_t f2_ret;
	f(f2_ret);
	PEXPECT_EQ(f1_ret.first, f2_ret.first);
	PEXPECT_EQ(f1_ret.second, 3);
	PEXPECT_EQ(f2_ret.second, 4);

	// vti2=0xdf6d50.vp1
	// vti2=0xdf6d50.vp2
	ptr.reset(new vti2);
	f = test_delegate(&vtb::vp1, ptr.get());
	f(f1_ret);
	f = test_delegate(&vtb::vp2, ptr.get());
	f(f2_ret);
	PEXPECT_EQ(f1_ret.first, f2_ret.first);
	PEXPECT_EQ(f1_ret.second, 13);
	PEXPECT_EQ(f2_ret.second, 14);

	//vb=0x63fafc
	//vd1=0x63fae0
	//vd2=0x63faf0
	//vd1=0x63fae0(8)
	//vd2=0x63faf0(9)

	forward obj2;
	obj2.x = 7;
	obj2.y = 8;
	obj2.z = 9;
	PEXPECT_NE(static_cast<void *>(static_cast<vb *>(&obj2)), static_cast<void *>(static_cast<vd1 *>(&obj2)));
	PEXPECT_NE(static_cast<void *>(static_cast<vb *>(&obj2)), static_cast<void *>(static_cast<vd2 *>(&obj2)));

#if defined(_MSC_VER) && !defined(__clang__)
	f = make_forward_delegate(&forward::xp, &obj2);
	f(fr);
	PEXPECT_TRUE(fr.second == 7);
	std::cout << fr.second << "\n";
#endif
	f = make_forward_delegate(&forward::yp, &obj2);
	f(fr);
	PEXPECT_EQ(fr.second, 8);
	f = make_forward_delegate(&forward::zp, &obj2);
	f(fr);
	PEXPECT_EQ(fr.second, 9);
}
