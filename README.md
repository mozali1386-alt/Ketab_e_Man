# کتاب من (Ketab-e-Man)

یک فروشگاه آنلاین کتاب دسکتاپ مبتنی بر معماری **کلاینت–سرور** که با **Qt 6** و **C++** پیاده‌سازی شده است.

کاربران عادی می‌توانند کتاب بخرند، مطالعه کنند و کتابخانه شخصی بسازند؛ ناشران کتاب‌های خود را مدیریت و منتشر می‌کنند؛ و ادمین بر کل سیستم نظارت دارد.

---

## ویژگی‌ها

### نقش‌ها
| نقش | قابلیت‌ها |
|-----|----------|
| **کاربر عادی (Normal User)** | ثبت‌نام، ورود، جستجو و خرید کتاب، سبد خرید، کیف پول، کتابخانه شخصی، قفسه، ذخیره کتاب، دانلود و مطالعه PDF، ثبت نظر و امتیاز |
| **ناشر (Publisher)** | ثبت‌نام و ورود، افزودن/ویرایش/حذف کتاب، آپلود PDF و تصویر جلد، آمار فروش، پروفایل ناشر |
| **ادمین (Admin)** | مدیریت کاربران و ناشران، مدیریت کتاب‌ها، حذف نظرات، داشبورد آماری |

### امکانات اصلی
- فروشگاه کتاب با جستجو و فیلتر بر اساس ژانر
- سبد خرید و پرداخت از کیف پول
- کتابخانه شخصی با امکان ساخت قفسه (Shelf)
- نمایش‌دهنده PDF داخلی
- سیستم نظرات و امتیازدهی
- اعلان‌ها (کتاب جدید در ژانر مورد علاقه، تخفیف، فروش جدید و …)
- آپلود فایل PDF به‌صورت قطعه‌ای (Chunked)
- داشبورد سرور با لاگ زنده

### ژانرهای پشتیبانی‌شده
Romance · Crime · Sci-Fi · Horror · Classic · Art · Comedy · History

---

## ساختار پروژه

```
Ketab_e_Man-master/
├── client/                 # برنامه کلاینت (GUI)
│   ├── main.cpp
│   ├── loginwindow.*       # ورود
│   ├── signinwindow.*      # ثبت‌نام کاربر
│   ├── publishersignupwindow.*
│   ├── storepage.*         # فروشگاه
│   ├── bookdetails.*
│   ├── cartpage.*
│   ├── librarywidget.*     # کتابخانه کاربر
│   ├── pdfviewerwidget.*
│   ├── userdashboard.*
│   ├── publisherdashboard.*
│   ├── admindashboard.*
│   ├── clientsocketmanager.*  # ارتباط شبکه با سرور
│   └── images/
│
├── S/                      # سرور + مدل‌های مشترک
│   ├── CMakeLists.txt
│   ├── server/
│   │   ├── main.cpp
│   │   ├── ServerCore.*       # هسته منطق کسب‌وکار
│   │   ├── ServerCore_*.cpp   # ماژول‌های جدا (Auth, Books, Shopping, …)
│   │   ├── ServerDataStore.*
│   │   ├── network/           # ServerNetwork, ClientHandler
│   │   ├── database/          # FileManager (ذخیره‌سازی مبتنی بر فایل)
│   │   └── gui/               # ServerDashboard
│   └── shared/                # مدل‌ها و پروتکل مشترک
│       ├── Protocol.*
│       ├── Enums.*
│       ├── User, NormalUser, Publisher, Admin
│       ├── Book, Library, Shelf, Cart, Review
│       ├── Wallet, Transaction, Purchase, Notification
│       └── …
│
└── uml.svg                 # دیاگرام UML
```

---

## پیش‌نیازها

- **CMake** ≥ 3.19 (کلاینت) / ≥ 3.30 (سرور)
- **Qt 6.5+** با ماژول‌های زیر:
  - Core, Gui, Widgets, Network
  - Charts
  - Pdf, PdfWidgets
  - Quick, QuickWidgets
- کامپایلر C++20 (مثلاً MSVC، MinGW یا GCC/Clang)
- سیستم‌عامل: Windows / Linux / macOS

---

## ساخت و اجرا

### ۱) سرور

```bash
cd S
mkdir build && cd build
cmake ..
cmake --build .
```

اجرای سرور:

```bash
./server/Ketab_e_Man_Server   # لینوکس/مک
# یا
server\Ketab_e_Man_Server.exe  # ویندوز
```

سرور روی پورت **5050** گوش می‌دهد و یک داشبورد گرافیکی نمایش می‌دهد.

### ۲) کلاینت

```bash
cd client
mkdir build && cd build
cmake ..
cmake --build .
```

اجرای کلاینت:

```bash
./Ketab_e_Man
```

کلاینت به‌صورت پیش‌فرض به `127.0.0.1:5050` وصل می‌شود (قابل تغییر در `main.cpp`).

> **نکته:** ابتدا سرور را اجرا کنید، سپس کلاینت را باز کنید.

---

## پروتکل ارتباطی

ارتباط کلاینت و سرور بر اساس یک پروتکل متنی ساده روی TCP است:

```
COMMAND_NAME||field1||field2||...
```

- جداکننده فیلدها: `||`
- لیست‌های داخل یک فیلد با `,` جدا می‌شوند
- تصویر جلد کتاب به‌صورت **base64** ارسال می‌شود
- فایل PDF با پروتکل قطعه‌ای (`UPLOAD_PDF_START` / `UPLOAD_PDF_CHUNK` / `UPLOAD_PDF_END`) منتقل می‌شود

نمونه‌ها:

```
LOGIN||NORMALUSER||username||password
SIGNUP_PUBLISHER||publisherName||username||email||password
SEARCH_STOREPAGE||query||genre
ADD_CART||bookId
```

جزئیات کامل پروتکل در فایل `ProtocolReference newer.md` (در پوشه بالاتر پروژه) آمده است.

---

## ذخیره‌سازی داده

سرور از ذخیره‌سازی مبتنی بر فایل (`FileManager`) استفاده می‌کند. داده‌ها در مسیر `assets/data` کنار اجرایی سرور نگهداری می‌شوند. نیازی به دیتابیس خارجی نیست.

---

## معماری

```
┌─────────────┐         TCP :5050         ┌──────────────────┐
│   Client    │ ◄──────────────────────► │     Server       │
│  (Qt GUI)   │   Protocol (|| text)     │  ServerCore +    │
│             │                           │  ClientHandler   │
└─────────────┘                           │  FileManager     │
                                          └──────────────────┘
```

- **کلاینت:** رابط کاربری با Qt Widgets + چند ویجت QML؛ مدیریت اتصال از طریق `ClientSocketManager`
- **سرور:** منطق کسب‌وکار در `ServerCore` (تقسیم‌شده به ماژول‌های Auth، Books، Shopping، Library و …)؛ هر اتصال کلاینت توسط یک `ClientHandler` مدیریت می‌شود
- **shared:** مدل‌های دامنه و تعریف پروتکل مشترک بین کلاینت و سرور

---

## لایسنس

این پروژه برای اهداف آموزشی/دانشگاهی توسعه یافته است.
