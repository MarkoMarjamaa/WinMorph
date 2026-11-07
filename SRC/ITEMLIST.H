/**********************************************************************
 * 
 *  NAME:         ITEMLIST.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 *
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          23.01.96 MHM created
 * 
 *********************************************************************/ 
 
#ifndef ITEMLIST_H
#define ITEMLIST_H

#include "common.h"
#ifndef __WINDOWS_H
	#include <windows.h>
#endif

// Selection List flags
#define SL_SELECTED		16384
// Bits 0 - 14 are for the connected count

int GetUniqueID(void);

class ItemList
{
	int nAllocationThreshold;

	int nItems;
	int *Item;
	int nItemsAllocated;

	void	Resize		(int nNewSize);
public:
	ItemList(void);
	ItemList(ItemList*);
	~ItemList();

	int		GetItems	(void)	{ return nItems; };

	inline  int 	GetItem		(int nIndex);
    inline  void	SetItem		(int nIndex,int  nValue);

	void	AddItem		(int  nValue);
	void	DeleteLastItem(void);
	void	DeleteItem	(int nIndex);

	void	SwapItems	(int nIndex1,int nIndex2);
    int		SearchItem	(int  nValue);
	void 	Reset		(void);

	inline  void	SetFlag		(int nIndex,int nFlag,BOOL nValue);
	inline  BOOL	GetFlag		(int nIndex,int nFlag);
};

inline int ItemList::GetItem(int nIndex)
{
	return Item[nIndex];
};

inline void ItemList::SetItem(int nIndex,int nValue)
{
	Item[nIndex]= nValue;
};

inline void ItemList::SetFlag(int nIndex,int nFlag,BOOL nValue)
{
    // Remove bit
	Item[nIndex] &= ( -1 - nFlag );
    // Set bit 
	if(nValue)
		Item[nIndex] |= nFlag;
};

inline BOOL ItemList::GetFlag(int nIndex,int nFlag)
{
	return (TRUE&&(Item[nIndex]&nFlag));
};

#endif

/* End of file ITEMLIST.H */


