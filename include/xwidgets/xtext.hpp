/***************************************************************************
* Copyright (c) 2017, Sylvain Corlay and Johan Mabille                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XWIDGETS_TEXT_HPP
#define XWIDGETS_TEXT_HPP

#include "xstring.hpp"

namespace xeus
{
    /********************
     * text declaration *
     ********************/

    template <class D>
    class xtext : public xstring<D>
    {
    public:

        using submit_callback_type = std::function<void()>;

        using base_type = xstring<D>;
        using derived_type = D;

        xtext();
        xjson get_state() const;
        void apply_patch(const xjson& patch);

        void on_submit(submit_callback_type);

        XPROPERTY(bool, derived_type, disabled);
        XPROPERTY(bool, derived_type, continuous_update, true);

        void handle_custom_message(const xjson&);

    private:

        void set_defaults();

        std::list<submit_callback_type> m_submit_callbacks;
    };

    class text final : public xtext<text>
    {
    public:

        using base_type = xtext<text>;

        text()
            : base_type()
        {
            this->open();
        }

        ~text()
        {
            if (!this->moved_from())
            {
                this->close();
            }
        }

        text(const text& other)
            : base_type(other)
        {
            this->open();
        }

        text& operator=(const text& other)
        {
            base_type::operator=(other);
            this->open();
            return *this;
        }

        text(text&&) = default;
        text& operator=(text&&) = default;
    };

    /************************
     * xtext implementation *
     ************************/

    template <class D>
    inline xtext<D>::xtext()
        : base_type()
    {
        set_defaults();
    }

    template <class D>
    inline xjson xtext<D>::get_state() const
    {
        xjson state = base_type::get_state();

        XOBJECT_SET_PATCH_FROM_PROPERTY(disabled, state);
        XOBJECT_SET_PATCH_FROM_PROPERTY(continuous_update, state);

        return state;
    }

    template <class D>
    inline void xtext<D>::apply_patch(const xjson& patch)
    {
        base_type::apply_patch(patch);

        XOBJECT_SET_PROPERTY_FROM_PATCH(disabled, patch)
        XOBJECT_SET_PROPERTY_FROM_PATCH(continuous_update, patch)
    }

    template <class D>
    inline void xtext<D>::on_submit(submit_callback_type cb)
    {
        m_submit_callbacks.emplace_back(std::move(cb));
    }

    template <class D>
    inline void xtext<D>::set_defaults()
    {
        this->_model_name() = "TextModel";
        this->_view_name() = "TextView";
    }

    template <class D>
    inline void xtext<D>::handle_custom_message(const xjson& content)
    {
        auto it = content.find("event");
        if (it != content.end() && it.value() == "submit")
        {
            for (auto it = m_submit_callbacks.begin(); it != m_submit_callbacks.end(); ++it)
            {
                it->operator()();
            }
        }
    }
}

#endif