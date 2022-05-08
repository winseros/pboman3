#pragma once

#include <ShObjIdl.h>
#include "../comobject.h"

namespace pboman3 {
    template <class TImplementation, class... TInterface>
    class AbstractEnumerator : public ComObject<TImplementation, IEnumExplorerCommand, TInterface...> {
    public:
        AbstractEnumerator(): index_(0) {
        }

        //IEnumExplorerCommand

        HRESULT Next(ULONG celt, IExplorerCommand** pUICommand, ULONG* pceltFetched) override {
            ULONG itemsFetched = 0;
            while (hasNext() && itemsFetched < celt) {
                pUICommand[itemsFetched] = createForIndex(index_);
                index_++;
                itemsFetched++;
            }
            if (pceltFetched)
                *pceltFetched = itemsFetched;

            return itemsFetched > 0 ? S_OK : S_FALSE;
        }

        HRESULT Skip(ULONG celt) override {
            return E_NOTIMPL;
        }

        HRESULT Reset() override {
            index_ = 0;
            return S_OK;
        }

        HRESULT Clone(IEnumExplorerCommand** ppenum) override {
            return E_NOTIMPL;
        }

    protected:
        virtual IExplorerCommand* createForIndex(ULONG index) const = 0;

        virtual ULONG numberOfItems() const = 0;

    private:
        ULONG index_;

        bool hasNext() const {
            return index_ < numberOfItems();
        }
    };
}
