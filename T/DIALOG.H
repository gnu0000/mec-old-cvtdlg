/* 
 * DIALOG.H 
 */ 

#define DIALOG_TITLE          99
#define ADD0_START            100
#define IDOK                  2
#define IDCANCEL              3
#define EDIT_BUTTON           3

#define ADD_ADDMORE           (ADD0_START)
#define ADDST_NEWSHEETNAME    (ADD0_START + 1)
#define ADD_NEWSHEETNAME      (ADD0_START + 2)

#define ADD1_START            (ADD0_START + 100)
#define ADDST_ITEMNUMBER      (ADD1_START)
#define ADD_ITEMNUMBER        (ADD1_START + 1)
#define ADDST_ADDLIST         (ADD1_START + 2)
#define ADD_ADDLIST           (ADD1_START + 3)
#define ADDST_QUANTITY        (ADD1_START + 4)
#define ADD_QUANTITY          (ADD1_START + 5)
#define ADD_SHPRICEINFO       (ADD1_START + 6)
#define ADD1_END              (ADD_SHPRICEINFO)

#define ADD11_START           (ADD1_START + 100)
#define ADDST_ITEMDESCRIPTION (ADD11_START)
#define ADD_ITEMDESCRIPTION   (ADD11_START + 1)
#define ADDST_UNIT            (ADD11_START + 2)
#define ADD_UNIT              (ADD11_START + 3)
#define ADD11_END             (ADD_UNIT)

#define ADD12_START           (ADD11_START + 100)
#define ADDST_UNITPRICE       (ADD12_START)
#define ADD_UNITPRICE         (ADD12_START + 1)
#define ADD12_END             (ADD_UNITPRICE)

#define ADD2_START            (ADD12_START + 100)
#define ADD_PRICEBASIS        (ADD2_START)
#define ADDST_SHEETNAMELIST   (ADD2_START + 1)
#define ADD_SHEETNAMELIST     (ADD2_START + 2)
#define ADD_REMOVEPRICE       (ADD2_START + 3)
#define ADD_ATTACHPRICE       (ADD2_START + 4)
#define ADD2_END              (ADD_ATTACHPRICE)

#define ADD_ASMALT            (ADD2_START + 5)

#define ADD21_START           (ADD2_START + 100)
#define COSHSHEET_BUTTON      (ADD21_START)
#define BIDHISTORY_BUTTON     (ADD21_START + 1)
#define REFERENCE_BUTTON      (ADD21_START + 2)
#define ASMITEMLIST_BUTTON    (ADD21_START + 3)
#define ADDHOC_BUTTON         (ADD21_START + 4)
#define ADD21_END             (ADDHOC_BUTTON)

