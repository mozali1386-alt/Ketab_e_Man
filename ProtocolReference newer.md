# مرجع کامل پروتکل کلاینت-سرور — Ketab-e-Man

⚠️ این سند حاصل ترکیب دو نسخه‌ست: نسخه‌ای که از رو کد فعلی سرور استخراج شده بود + نسخه‌ای که دستورهای جدیدتر (SIGNUP، SEARCH_STOREPAGE، base64، بخش ادمین) رو اضافه کرده بود. جاهایی که این دو تناقض داشتن یا نسخه‌ی جدید مبهم بود، صریح مشخص شده — فایل‌های واقعی پروژه هنوز طبق طراحی خیلی قدیمی‌تر (commandId عددی) هستن و باید کامل بازنویسی بشن تا با این سند هماهنگ بشن.

> **فرمت پیام:** دیگه commandId عددی وجود نداره. هر پیام یه رشته‌ی متنی واحده که فیلد اولش نام دستوره (رشته‌ای، حروف بزرگ) و بقیه‌ی فیلدها با `||` از هم جدا می‌شن.
> ```cpp
> QString msg = QString("LOGIN||%1||%2||%3").arg(role, username, password);
> ```

## قوانین کلی (برای همه‌ی درخواست‌ها)

- فرمت کلی: `COMMAND_NAME||field1||field2||...`
- سرور اسم دستور رو از توکن اول (قبل از اولین `||`) تشخیص می‌ده؛ باید دقیقاً case-sensitive با چیزی که سرور می‌شناسه یکی باشه.
- لیست‌های داخل یه فیلد (چند ژانر، چند bookId) با `,` (کاما) جدا می‌شن.
- درخواست‌های 🔒 نیاز به لاگین دارن — سرور `userId`/`adminId` رو از سشن (`handler->getUserId()`) می‌گیره، نه از payload. **این یعنی سرور باید موقع لاگین موفق، `userId` رو رو خودِ شیء `ClientHandler` (یا معادلش) همون کانکشن ذخیره کنه و تا وقتی همون کلاینت وصله (یا لاگ‌اوت/قطع نشه) نگهش داره.** مثلاً `ADD_CART||bookId`, `REMOVE_CART||bookId`, `SAVE_BOOK||bookId` و بقیه‌ی دستورهای 🔒 هیچ‌کدوم `userId` رو تو payload نمی‌فرستن — تنها راهی که سرور می‌فهمه این درخواست مال کدوم کاربره، همین state ذخیره‌شده‌ی رو کانکشنه.
- شکست احراز هویت: `FAIL||"Not authenticated"`
- کوتاه بودن تعداد فیلدها: `FAIL||"Invalid data"`
- پاسخ‌های سرور هم همین فرمت رو دارن: `RESULT_NAME||field1||field2||...`

### فرض: کاراکتر `|` هیچ‌وقت تو فیلدهای آزاد نیست
کاراکتر `|` هیچ‌وقت داخل هیچ فیلد متنی آزادی (نام، عنوان، توضیحات، متن نظر، رمز عبور، و غیره) ظاهر نمی‌شه. پس نیازی به escape/unescape (`PipeEscape`) نیست؛ فیلدها همون‌طور که هستن ارسال می‌شن و سرور فقط با split کردن روی `||` پارسشون می‌کنه.

### تصاویر و فایل PDF
عکس جلد کتاب همیشه به‌صورت **base64 کامل تو یه فیلد** ارسال/دریافت می‌شه (نه مسیر فایل) — مثل `coverImageBase64` تو `ADD_BOOK_METADATA`/`EDIT_BOOK`/`PUB_BOOK_DETAILS_RESULT`/`BOOK_SUMMARY`. فایل **pdf** ولی دیگه این‌طوری نیست: چون معمولاً خیلی بزرگ‌تره، از یه پروتکل **chunked** جدا (`UPLOAD_PDF_START`/`UPLOAD_PDF_CHUNK`/`UPLOAD_PDF_END`، بخش ۲ رو ببین) استفاده می‌کنه، نه یه فیلد base64 تو همون پیام اصلی.

### الگوی «مشاهدات» (list → detail)
برای هر بخشی که کاربر باید یه‌سری آیتم رو ببینه، پروتکل دو مرحله‌ایه:
1. یه درخواست لیست می‌فرسته، در جواب فقط **لیست id** می‌گیره (نه اطلاعات کامل).
2. اگه جزئیات کامل یه آیتم خاص لازم بود، همون id رو تو یه درخواست جدا (`..._DETAIL`/`GET_..._SUMMARY`) دوباره می‌فرسته.

این الگو رو `SEARCH_STOREPAGE`+`GET_BOOK_SUMMARY` پیاده می‌کنن؛ بخش‌های جدید ادمین (لیست کاربران، جزئیات کاربر/ناشر، لیست کتاب‌ها) هم باید همین الگو رو رعایت کنن.

### قرارداد نام‌گذاری پاسخ‌ها: `<COMMAND>_RESULT`
خیلی از دستورهای جدیدتر سند (مثل `ADD_SHELF`, `EDIT_SHELF`, `DELETE_SHELF`, `UNSAVE_BOOK`, `ASSIGN_TO_SHELF`, `CHECKOUT_CART`, `UPDATE_PROFILE`, و لیست‌های `GET_MY_BOOKS`/`GET_SAVED_BOOKS`/`GET_SHELVES`/`GET_SHELF_BOOKS`) به‌جای پاسخ ژنریک `SUCCESS`/`FAIL`، پاسخ خودشون رو با اسم مخصوص `<COMMAND>_RESULT` برمی‌گردونن (مثلاً `ADD_SHELF_RESULT||SUCCESS`). این یعنی هرچی جلوتر می‌ریم سند، این الگو بیشتر از `SUCCESS`/`FAIL` ساده‌ی اولیه رایج‌تر می‌شه — پس موقع پیاده‌سازی، لیست کامل «جدول خلاصه‌ی پاسخ‌ها» ته سند رو چک کنید، نه فقط `SUCCESS`/`FAIL`.

---

## ۱) احراز هویت و حساب کاربری

### `LOGIN`
**نیاز به لاگین:** ندارد

**رشته‌ی ارسالی (۳ فیلد):**
```
LOGIN||role||username||password
```
(`role`: یکی از `ADMIN`, `NORMALUSER`, `PUBLISHER` (رشته‌ای، case-sensitive) — نوع حسابی که کلاینت قصد داره باهاش وارد بشه)

**پاسخ موفق:**
```
LOGIN||SUCCESS||userId
```

**پاسخ شکست:**
```
LOGIN||FAIL||Invalid username or password
```

⚠️ **استثنا:** برخلاف بقیه‌ی دستورها، پاسخ `LOGIN` با نام خودِ دستور شروع می‌شه (نه `SUCCESS`/`FAIL` تنها). `role` تو پاسخ برنمی‌گرده.

**نکته:** اگه همین حساب از سشن دیگه‌ای هم لاگین باشه، اون سشن قبلی خودکار خارج می‌شه: `LOGIN||FAIL||Logged in from another location`

---

### `SIGNUP_PUBLISHER`
**نیاز به لاگین:** ندارد

**رشته‌ی ارسالی (۴ فیلد):**
```
SIGNUP_PUBLISHER||publisherName||username||email||password
```

**پاسخ موفق:** `SIGNUP_PUBLISHER||SUCCESS`

**پاسخ‌های شکست:**
```
SIGNUP_PUBLISHER||FAIL||USERNAME_EXISTS
SIGNUP_PUBLISHER||FAIL||EMAIL_EXISTS
```
**تأیید شد:** اگه هر دو (username و email) هم‌زمان تکراری باشن، فقط **یکی** از این دو کد برمی‌گرده، هیچ‌وقت هر دو با هم (احتمالاً اول username چک می‌شه).

---

### `SIGNUP_NORMALUSER`
**نیاز به لاگین:** ندارد

**رشته‌ی ارسالی (۵ فیلد):**
```
SIGNUP_NORMALUSER||fullName||username||email||password||genresString
```
(`genresString`: بین ۱ تا ۳ ژانر با `,` جدا شده. مقادیر معتبر (رشته‌ای، case-sensitive، همه‌حروف‌بزرگ): `ROMANCE`, `CRIME`, `SCIFI`, `HORROR`, `CLASSIC`, `ART`, `COMEDY`, `HISTORY` — مثلاً `ROMANCE,SCIFI,ART`. این تنها راهیه که ژانر موردعلاقه‌ی کاربر عادی ثبت می‌شه (لحظه‌ی ثبت‌نام) — دستور جدای بعدی برای تغییرش وجود نداره. همین ۸ مقدار، همه‌جای سند که «genre» هست (`SEARCH_STOREPAGE`, `GET_BOOK_INFO_ADMIN`) استفاده می‌شه — enum عددی قبلی کنار گذاشته شد.

**پاسخ موفق:** `SIGNUP_NORMALUSER||SUCCESS`

**پاسخ‌های شکست:**
```
SIGNUP_NORMALUSER||FAIL||USERNAME_EXISTS
SIGNUP_NORMALUSER||FAIL||EMAIL_EXISTS
```
(همون نکته‌ی بالا درباره‌ی ترکیب دو کد خطا اینجا هم صادقه.)

---

### `FORGOT_PASS_CHECK`
**نیاز به لاگین:** ندارد

**رشته‌ی ارسالی:** `FORGOT_PASS_CHECK||email`

**پاسخ موفق:** `FORGOT_PASS_CHECK||SUCCESS||username`

**پاسخ شکست:** `FORGOT_PASS_CHECK||FAIL` (بدون فیلد علت)

---

### `FORGOT_PASS_UPDATE`
**نیاز به لاگین:** ندارد (ولی به سشن قبلی وابسته — پایین رو ببین)

**رشته‌ی ارسالی:** `FORGOT_PASS_UPDATE||newPassword`

**پاسخ موفق:** `FORGOT_PASS_UPDATE||SUCCESS`
**پاسخ شکست:** `FORGOT_PASS_UPDATE||FAIL`

**نکته‌ی معماری مهم:** این دستور هیچ فیلدی نداره که بگه رمز *کدوم* کاربر عوض بشه. سرور کاربر رو از رو **همون کانکشن/سشنی** که `FORGOT_PASS_CHECK` رو ازش گرفته به خاطر می‌سپاره (یه mapping موقت `ClientHandler* → userId در حال بازیابی`، جدا از `loggedInClients` معمولی). **تأیید شد: قراره همین‌جوری بمونه** — بدون فیلد اضافه‌ی `username`/`email`. یعنی کلاینت مطلقاً نباید بین این دو درخواست قطع/وصل بشه؛ اگه قطع بشه (مثلاً قطعی نت)، کل فرآیند بازیابی رمز باید از اول انجام بشه — این یه محدودیتِ پذیرفته‌شده‌ست، نه باگ.

🔑 **پیش‌نیاز اجباری برای این الگو:** هر شیء `ClientHandler` سمت سرور باید بتونه *به‌طور مستقل* بدونه این درخواست الان از کدوم کاربره — یعنی هر `ClientHandler` باید یه فیلد عضو (state) داشته باشه که `userId`ِ در حال بازیابیِ همون کانکشن رو نگه داره (جدا از فیلد `userId` معمولیِ سشن لاگین‌شده). این باید موقع پیاده‌سازی صریح تست بشه: باید مطمئن بشیم `ClientHandler`های مختلف با هم قاطی نمی‌شن (که چون هرکدوم شیء جدا هستن، طبیعتاً مشکلی نداره، ولی باید موقع پیاده‌سازی صریح تست بشه).

🔑 **پیش‌نیاز اجباری برای این الگو:** هر شیء `ClientHandler` سمت سرور باید بتونه *به‌طور مستقل* بدونه این درخواست الان از کدوم کاربره — یعنی هر `ClientHandler` باید یه فیلد عضو (state) داشته باشه که `userId`ِ در حال بازیابیِ همون کانکشن رو نگه داره (جدا از فیلد `userId` معمولیِ سشن لاگین‌شده). این باید موقع پیاده‌سازی صریح تست بشه: باید مطمئن بشیم `ClientHandler`های مختلف با هم قاطی نمی‌شن (که چون هرکدوم شیء جدا هستن، طبیعتاً مشکلی نداره، ولی باید موقع پیاده‌سازی صریح تست بشه).

---

### `LOGOUT` 🔒
**رشته‌ی ارسالی:** `LOGOUT` (بدون فیلد)
**پاسخ:** `SUCCESS||"Logged out"`

---

## ۲) کتاب و ناشر

### `GET_PUBLISHER_BOOKS` 🔒 — فقط Publisher
کاربرد: لیست کتاب‌های خودِ ناشر، برای نمایش تو ComboBox.

**رشته‌ی ارسالی:** `GET_PUBLISHER_BOOKS` (بدون فیلد؛ `userId`/`publisherId` از سشن)

**پاسخ:** `PUBLISHER_BOOKS_RESULT||totalCount||bookId:bookName,bookId:bookName,...`
مثال: `PUBLISHER_BOOKS_RESULT||2||101:هری پاتر,102:سی پلاس پلاس`

---

### `GET_PUB_BOOK_DETAILS` 🔒 — نه برای کاربر عادی
کاربرد: پر کردن فرم ویرایش وقتی ناشر رو یکی از کتاب‌هاش کلیک می‌کنه (شامل عکس جلد). **تأیید شد: مخصوص پنل ناشر/ادمینه، برای کاربر عادی (فروشگاه) کاربرد نداره.**

**رشته‌ی ارسالی:** `GET_PUB_BOOK_DETAILS||bookId`

**پاسخ:** `PUB_BOOK_DETAILS_RESULT`
```
PUB_BOOK_DETAILS_RESULT||bookId||bookName||authorName||genre||price||discount||explanation||isActive||imageBase64
```
مثال: `PUB_BOOK_DETAILS_RESULT||101||هری پاتر||جی کی رولینگ||تخیلی||150000||10||کتابی عالی||1||iVBORw0KGgoAAA...`

(`isActive`: سرور هم عدد (`1`/`0`) هم کلمه (`TRUE`/`FALSE`) رو قبول می‌کنه — هرکدوم رو بفرستی مشکلی نیست. **تأیید شد:** این با `status` (`ACTIVE`/`INACTIVE` تو `BOOK_INFO_ADMIN_RESULT`) و فعل‌های `ENABLE`/`DISABLE` (تو `TOGGLE_BOOK_STATUS`) همه یه فیلدِ واحدِ فعال/غیرفعالِ کتابن، فقط تو هر endpoint به شکل متفاوتی serialize می‌شن — نه سه‌تا فیلدِ جدا تو دیتابیس.)

---

### افزودن کتاب جدید (دو فاز)

**فاز ۱ — متادیتا + عکس:**
```
ADD_BOOK_METADATA||bookName||authorName||genre||price||discount||explanation||coverImageBase64
```
**پاسخ موفق:** `ADD_BOOK_RESULT||SUCCESS||generatedBookId`

سرور کتاب رو با همین اطلاعات تو دیتابیس ثبت می‌کنه و یه `bookId` جدید می‌سازه؛ کلاینت باید همین id رو تو فاز ۲ (آپلود pdf) استفاده کنه.

**فاز ۲ — آپلود pdf (تکه‌تکه/chunked):**
```
UPLOAD_PDF_START||generatedBookId||totalChunks
UPLOAD_PDF_CHUNK||generatedBookId||base64ChunkData      (به تعداد totalChunks، تکراری)
UPLOAD_PDF_END||generatedBookId
```
**پاسخ موفق (بعد از END):** `UPLOAD_PDF_RESULT||SUCCESS`

سرور تکه‌ها رو به هم می‌چسبونه و موقع دریافت `UPLOAD_PDF_END` به فایل باینری pdf تبدیلشون می‌کنه (و تعداد صفحات رو هم همون‌جا استخراج/ذخیره می‌کنه).

⚠️ **جایگزینِ کاملِ `PUBLISH` قدیمی:** قبلاً یه دستور تک‌مرحله‌ای `PUBLISH||title||authorName||genre||description||price||coverImageBase64||pdfBase64` مستند شده بود (که کل pdf رو base64-شده تو یه فیلد می‌فرستاد). این پروتکل دو-فازی + chunked، اون رو منسوخ می‌کنه — `PUBLISH` دیگه استفاده نمی‌شه.

---

### ویرایش کتاب موجود
```
EDIT_BOOK||bookId||bookName||authorName||genre||price||discount||explanation||coverImageBase64
```
**پاسخ موفق:** `EDIT_BOOK_RESULT||SUCCESS`

**نکته:** اگه ناشر عکس جدید انتخاب نکرده باشه، کلاینت تو فیلد آخر کلمه‌ی `EMPTY` می‌فرسته؛ سرور تو این حالت نباید عکس قبلی رو پاک کنه.

⚠️ **جایگزینِ کاملِ `UPDATE_BOOK` قدیمی:** قبلاً `UPDATE_BOOK||bookId||newPrice||newDiscount` (فقط قیمت/تخفیف) مستند شده بود. `EDIT_BOOK` جامع‌تره (همه‌ی فیلدهای کتاب رو با هم عوض می‌کنه) و جایگزین کاملشه — `UPDATE_BOOK` دیگه استفاده نمی‌شه.

---

### تغییر وضعیت نمایش کتاب (فعال/غیرفعال)
```
TOGGLE_BOOK_STATUS||bookId||DISABLE
TOGGLE_BOOK_STATUS||bookId||ENABLE
```
**پاسخ موفق:** `TOGGLE_STATUS_RESULT||SUCCESS`

**نکته:** جایگزینِ کاملِ `DEACTIVATE_BOOK` قدیمیه (که `activateFlag` عددی داشت) — `DEACTIVATE_BOOK` دیگه استفاده نمی‌شه. **تأیید شد:** قانون قبلی همچنان برقراره — اگه Admin کتابی رو حذف کرده باشه، ناشر نمی‌تونه با `ENABLE` دوباره فعالش کنه.

---

### `DELETE_BOOK` 🔒 — فقط Admin
**رشته‌ی ارسالی:** `DELETE_BOOK||bookId`
**پاسخ:** `DELETE_BOOK_RESULT||bookId||SUCCESS` یا `DELETE_BOOK_RESULT||bookId||FAILED`

**نکته:** معادل غیرفعال‌سازی دائمیه (به‌جز کتابخانه‌ی خریداران که دسترسیش می‌مونه)؛ همه‌ی کامنت‌های این کتاب هم کامل حذف می‌شن. (فرمت پاسخ نهایی همینه — نسخه‌ی قدیمی‌تر `SUCCESS||"Book deleted"` دیگه استفاده نمی‌شه.)

---

### `SEARCH_STOREPAGE` — 🔧 با فیلد `display` کامل مشخص‌شده

**نیاز به لاگین:** فقط وقتی `display=recommended` باشه 🔒

**رشته‌ی ارسالی (۵ فیلد):**
```
SEARCH_STOREPAGE||book||author||publisher||genre||display
```

**منطق ترکیب (مرحله‌ای):**

**مرحله‌ی ۱ — مجموعه‌ی پایه از ۴ فیلد اول:** هرکدوم از `book`(عنوان)/`author`(authorId)/`publisher`(publisherId)/`genre`(یکی از ۸ مقدار رشته‌ای معتبر — `ROMANCE`, `CRIME`, `SCIFI`, `HORROR`, `CLASSIC`, `ART`, `COMEDY`, `HISTORY`؛ بخش `SIGNUP_NORMALUSER` رو ببین) که پر باشه با AND ترکیب می‌شه. اگه هر ۴ تا `EMPTY` باشن → همه‌ی کتاب‌های فعال.

⚠️ **تغییر مهم — فیلدهای خالی دیگه بین دو `||` ول نمی‌شن؛ کلمه‌ی `EMPTY` صریح فرستاده می‌شه.** یعنی مثلاً `SEARCH_STOREPAGE||||||||||all` (فیلدهای خالی وسط) دیگه معتبر نیست؛ باید بشه `SEARCH_STOREPAGE||EMPTY||EMPTY||EMPTY||EMPTY||ALL`.

**مرحله‌ی ۲ — اعمال `display` روی همون مجموعه‌ی پایه (نه جایگزینش):**

⚠️ **مقادیر `display` عوض شدن (همه‌حروف‌بزرگ + یه مقدار جدید):**

| مقدار قدیمی | مقدار جدید | روی مجموعه‌ی پایه چی‌کار می‌کنه |
|---|---|---|
| (خالی) / `all` | `EMPTY` یا `ALL` | هیچی؛ همون مجموعه رو برمی‌گردونه |
| `newest` | `NEW` | مرتب می‌کنه بر اساس تاریخ انتشار، جدیدترین اول |
| `bestselling` | `BESTSELLER` | مرتب می‌کنه بر اساس تعداد فروش، پرفروش‌ترین اول |
| `popular` | `POPULAR` | مرتب می‌کنه بر اساس میانگین امتیاز نظرات، بالاترین اول |
| `recommended` 🔒 | `RECOMMENDED` 🔒 | با «کتاب‌های ژانرهای موردعلاقه‌ی کاربر لاگین‌کرده» تقاطع می‌گیره |
| — (جدید) | `FREE` | فقط کتاب‌های رایگان (قیمت نهایی صفر) |

**مثال‌ها (با فرمت جدید `EMPTY`):**
```
SEARCH_STOREPAGE||EMPTY||EMPTY||EMPTY||EMPTY||NEW              → جدیدترین‌ها، بدون فیلتر دیگه
SEARCH_STOREPAGE||EMPTY||EMPTY||EMPTY||HORROR||BESTSELLER      → پرفروش‌ترین‌های ژانر HORROR
SEARCH_STOREPAGE||Harry||EMPTY||EMPTY||EMPTY||RECOMMENDED      → از بین کتاب‌های "Harry"، فقط تو ژانر موردعلاقه‌ی من
SEARCH_STOREPAGE||EMPTY||EMPTY||EMPTY||EMPTY||ALL              → همه‌ی کتاب‌های فعال
SEARCH_STOREPAGE||Harry||EMPTY||EMPTY||EMPTY||EMPTY            → فقط فیلتر عنوان
SEARCH_STOREPAGE||EMPTY||EMPTY||EMPTY||EMPTY||FREE             → فقط کتاب‌های رایگان
```

**پاسخ:**
```
SEARCH_RESULT||bookId1,bookId2,bookId3
```
برای `NEW`/`BESTSELLER`/`POPULAR` ترتیب لیست مهمه. برای بقیه مهم نیست. اگه هیچی پیدا نشد، فیلد دوم خالی.

**پاسخ شکست (فقط `display=RECOMMENDED`):** `FAIL||"Not authenticated"`

**نکات پیاده‌سازی:** مرحله‌ی ۱ با `intersect()` روی `QSet<quint64>` (بعد از چک کردن `EMPTY` برای هر فیلد). برای `NEW`/`BESTSELLER`/`POPULAR` باید `QVector<quint64>` مرتب‌شده ساخته بشه (نه `QSet`، چون ترتیب مهمه): `NEW`→`getCreatedAt()` نزولی، `BESTSELLER`→`Book::getSalesCount()` نزولی، `POPULAR`→میانگین `stars` نظرات هر کتاب نزولی. `FREE`→فقط کتاب‌هایی که `finalPrice` صفره. برای `RECOMMENDED`: کتاب‌های هر ژانر موردعلاقه رو union کن، بعد با نتیجه‌ی مرحله‌ی ۱ intersect بگیر. برای جلوگیری از لیست خیلی طولانی، بهتره به یه تعداد ثابت (مثلاً ۲۰) محدود بشه.

---

### `GET_BOOK_SUMMARY`
**نیاز به لاگین:** ندارد

**رشته‌ی ارسالی:** `GET_BOOK_SUMMARY||bookId`

**پاسخ:**
```
BOOK_SUMMARY||bookId||coverImageBase64||title||authorName||finalPrice||average
```
مثال: `BOOK_SUMMARY||1||NO_IMAGE||سمفونی مردگان||عباس معروفی||130000||4.8`

(`coverImageBase64`: تصویر جلد base64-شده، یا `NO_IMAGE` اگه نداشت. `finalPrice`: قیمت نهایی بعد از تخفیف (نه قیمت پایه). `average`: میانگین امتیاز کامنت‌ها.)

این گام «detail» از الگوی list→detail هست: اول `SEARCH_STOREPAGE` لیست idها رو می‌ده، بعد کلاینت برای هر آیتمی که می‌خواد نمایش بده، `GET_BOOK_SUMMARY` رو با اون id صدا می‌زنه. (`GET_PUB_BOOK_DETAILS` که قبلاً بالای همین بخش کامل مستند شد، برای فرم ویرایش/جزئیات کاملِ ناشره — یه چیز جداست، نه یه مرحله‌ی دیگه از همین الگو.)

---

### `GET_BOOK_DETAILS` 🔒 — صفحه‌ی جزئیات کامل کتاب، مخصوص کاربر عادی
کاربرد: صفحه‌ی جزئیات کامل یه کتاب تو فروشگاه (نه فرم ویرایش ناشر — اون `GET_PUB_BOOK_DETAILS`ه).

**رشته‌ی ارسالی:** `GET_BOOK_DETAILS||bookId`

**پاسخ:** `BOOK_DETAILS_RESULT`
```
BOOK_DETAILS_RESULT||userId||username||imageBase64||title||authorName||publisherName||genre||price||discountPrice||average||description||isInCart||isInSaveBook||isPurchase
```
(`isInCart`/`isInSaveBook`/`isPurchase`: هرکدوم `0` یا `1`. `userId`/`username`: اطلاعات کاربرِ لاگین‌کرده‌ی درخواست‌دهنده، نه چیزی مربوط به کتاب.)

**تأیید شد:** فعاله (نه کامنت) و نیاز به لاگین داره — پس مشکل «بدون لاگین این فیلدها چی می‌فرستن» دیگه موضوعیت نداره.

⚠️ این ششمین فرمتِ «خلاصه/جزئیات کتاب» تو سنده (بعد از `BOOK_SUMMARY`, `CART_BOOK_SUMMARY`, `PUB_BOOK_DETAILS_RESULT`, `MY_BOOK_INFO_RESULT`, `SAVED_BOOK_INFO_RESULT`, `SHELF_BOOK_INFO_RESULT`) — عمدیه (هر صفحه فیلدهای خودش رو می‌گیره)، ولی حواستون به قاطی نشدنشون باشه.

---

## ۳) خرید و سبد خرید

### `BUY` 🔒
**رشته‌ی ارسالی:** `BUY||bookId1,bookId2,bookId3`
**پاسخ موفق:** `SUCCESS||purchaseId||totalAmount`
**پاسخ‌های شکست:** `FAIL||"Not authorized"` / `FAIL||"Library not found"` / `FAIL||"Wallet not found"` / `FAIL||"No books to purchase"` / `FAIL||"One of the books is unavailable"` / `FAIL||"One of the books has no valid publisher"` / `FAIL||"One of the publishers has no valid wallet"` / `FAIL||"Insufficient balance"`

---

### `GET_CART_ITEMS` 🔒
**رشته‌ی ارسالی:** `GET_CART_ITEMS` (بدون فیلد)

**پاسخ:** `CART_RESULT||count||bookId1,bookId2`
مثال‌ها:
```
CART_RESULT||2||1,3
CART_RESULT||EMPTY
```
(اگه سبد خالی باشه، به‌جای لیست خالی، مقدار خاصِ `EMPTY` برمی‌گرده — نه یه رشته‌ی خالی مثل بقیه‌ی جاهای سند. توجه: فرمت عوض شد — قبلاً فقط `CART_RESULT||1,3` بود، بدون فیلد تعداد؛ الان یه فیلد `count` قبل از لیست idها اضافه شده.)

این گام «list» از الگوی list→detail برای سبد خرید — گام «detail»ش `GET_CART_BOOK_SUMMARY` هست.

---

### `GET_CART_BOOK_SUMMARY` 🔒
**رشته‌ی ارسالی:** `GET_CART_BOOK_SUMMARY||bookId`

**پاسخ:** `CART_BOOK_SUMMARY`
```
CART_BOOK_SUMMARY||bookId||image||title||authorName||price||discountPrice
```
مثال: `CART_BOOK_SUMMARY||id||image||title||authorname||price||discountprice`

⚠️ **فرق با `BOOK_SUMMARY`:** اینجا برخلاف `BOOK_SUMMARY` (که فقط `finalPrice` رو می‌ده)، هم `price` (قیمت پایه) هم `discountPrice` (قیمت بعد از تخفیف) جدا جدا برمی‌گردن — و فیلد `average`/rating اصلاً نیست. یعنی این یه فرمت کاملاً جدا و مستقله، نه همون `BOOK_SUMMARY`.

---

### `ADD_CART` 🔒
**رشته‌ی ارسالی:** `ADD_CART||bookId`
**پاسخ:** `SUCCESS||"Added to cart"`

---

### `REMOVE_CART` 🔒
**رشته‌ی ارسالی:** `REMOVE_CART||bookId`
**پاسخ:** `SUCCESS||"Removed from cart"`

---

### `CHECKOUT_CART` 🔒
**رشته‌ی ارسالی:** `CHECKOUT_CART` (بدون فیلد)

**پاسخ موفق:** `CHECKOUT_RESULT||SUCCESS`
**پاسخ شکست:** `CHECKOUT_RESULT||FAILED`

**نکته:** چون `CHECKOUT_RESULT||SUCCESS` جزئیاتی نمی‌ده، کلاینت نیازی به یه درخواست جدا نداره — از قبل (از رو همون `GET_CART_ITEMS` که قبل از checkout زده) لیست bookIdهای سبد رو داره، و بعد از دریافت `SUCCESS` همون کتاب‌ها رو خودش (سمت UI) به کتابخانه اضافه می‌کنه.

---

## ۴) کتابخانه‌ی شخصی، قفسه‌ها، دانلود و کامنت‌ها

### `GET_MY_BOOKS` 🔒 — لیست کتاب‌های خریداری‌شده
**رشته‌ی ارسالی:** `GET_MY_BOOKS` (بدون فیلد؛ `userId` از سشن)
**پاسخ:** `MY_BOOKS_RESULT||TotalCount||BookID_1,BookID_2,...`

این گام «list»ه — گام «detail»ش `GET_MY_BOOK_INFO` (پایین) هست.

---

### `GET_SAVED_BOOKS` 🔒 — لیست علاقه‌مندی‌ها (Wishlist)
**رشته‌ی ارسالی:** `GET_SAVED_BOOKS` (بدون فیلد؛ `userId` از سشن)
**پاسخ:** `SAVED_BOOKS_RESULT||TotalCount||BookID_1,BookID_2,...`

گام «detail»ش `GET_SAVED_BOOK_INFO` (پایین) هست.

---

### `GET_SHELVES` 🔒 — لیست قفسه‌های کاربر
**رشته‌ی ارسالی:** `GET_SHELVES` (بدون فیلد؛ `userId` از سشن)
**پاسخ:** `SHELVES_RESULT||TotalCount||ShelfID:ShelfName,ShelfID:ShelfName,...`
(هر آیتم لیست خودش با `:` بین id و اسم قفسه جدا شده؛ آیتم‌ها با `,` از هم جدا می‌شن.)

---

### `GET_SHELF_BOOKS` 🔒 — لیست کتاب‌های داخل یه قفسه
**رشته‌ی ارسالی:** `GET_SHELF_BOOKS||shelfId`
**پاسخ:** `SHELF_BOOKS_RESULT||shelfId||BookID_1,BookID_2,...`
اگه خالی بود: `SHELF_BOOKS_RESULT||shelfId||EMPTY`

گام «detail»ش `GET_SHELF_BOOK_INFO` (پایین) هست.

---

### گرفتن اطلاعات یک کتاب — بسته به تب فعلی، ۳ دستور جدا

کلاینت برای گرفتن اطلاعات یه کتاب، بسته به این‌که تو کدوم تب/صفحه‌ست، یکی از این ۳ دستور رو می‌فرسته (نه یه دستور مشترک):

**الف) تب «کتاب‌های من»:**
`GET_MY_BOOK_INFO||bookId` → `MY_BOOK_INFO_RESULT||bookId||imageBase64||bookTitle||currentShelfId`

**ب) تب «ذخیره‌شده‌ها»** (بدون `currentShelfId`، ولی با وضعیت خرید):
`GET_SAVED_BOOK_INFO||bookId` → `SAVED_BOOK_INFO_RESULT||bookId||imageBase64||bookTitle||isPurchased`
(`isPurchased`: رشته‌ی `TRUE` یا `FALSE`، نه بولین واقعی و نه عدد.)

**ج) تب «قفسه‌ها»:**
`GET_SHELF_BOOK_INFO||bookId` → `SHELF_BOOK_INFO_RESULT||bookId||imageBase64||bookTitle||currentShelfId`

⚠️ این سه‌تا با `GET_BOOK_SUMMARY`/`BOOK_SUMMARY`، `GET_BOOK_DETAILS`/`BOOK_DETAILS_RESULT` (بخش ۲) و `CART_BOOK_SUMMARY` (بخش ۳) هم‌پوشانی دارن — یعنی الان ۶ تا فرمت «خلاصه/جزئیات کتاب» متفاوت داریم (`BOOK_SUMMARY`, `BOOK_DETAILS_RESULT`, `CART_BOOK_SUMMARY`, `MY_BOOK_INFO_RESULT`, `SAVED_BOOK_INFO_RESULT`, `SHELF_BOOK_INFO_RESULT`)، هرکدوم با فیلدهای کمی متفاوت، مخصوص همون صفحه‌ی خاص خودش. این عمداً این‌طوریه (هر تب دقیقاً فیلدهایی که لازم داره رو می‌گیره)، نه یه اشتباه — فقط حواستون باشه این‌ها را با هم قاطی نکنید.

---

### `ADD_SHELF` 🔒 — ساختن قفسه‌ی جدید
**رشته‌ی ارسالی:** `ADD_SHELF||newShelfName`
**پاسخ:** `ADD_SHELF_RESULT||SUCCESS`

**نکته:** جایگزینِ کاملِ `CREATE_SHELF` قدیمیه — `CREATE_SHELF` دیگه استفاده نمی‌شه.

---

### `EDIT_SHELF` 🔒 — تغییر اسم قفسه
**رشته‌ی ارسالی:** `EDIT_SHELF||shelfId||newShelfName`
**پاسخ:** `EDIT_SHELF_RESULT||SUCCESS`

---

### `DELETE_SHELF` 🔒
**رشته‌ی ارسالی:** `DELETE_SHELF||shelfId`
**پاسخ:** `DELETE_SHELF_RESULT||SUCCESS`

---

### `UNSAVE_BOOK` 🔒 — حذف از لیست علاقه‌مندی‌ها
**رشته‌ی ارسالی:** `UNSAVE_BOOK||bookId`
**پاسخ:** `REMOVE_SAVED_RESULT||SUCCESS` *(فرمت پاسخ همون قبلیه — فقط اسمِ خودِ دستور برگشت)*

**نکته:** این تصمیم برعکس شد — قبلاً `REMOVE_FROM_SAVED` جایگزینِ `UNSAVE_BOOK` شده بود؛ الان دوباره `UNSAVE_BOOK` نسخه‌ی نهاییه و `REMOVE_FROM_SAVED` دیگه استفاده نمی‌شه.

---

### `REMOVE_FROM_SHELF` 🔒 — حذف یه کتاب از یه قفسه‌ی خاص
**رشته‌ی ارسالی:** `REMOVE_FROM_SHELF||bookId||shelfId`
**پاسخ:** `REMOVE_FROM_SHELF_RESULT||SUCCESS`

---

### `ASSIGN_TO_SHELF` 🔒 — انتقال/تخصیص کتاب به یه قفسه‌ی جدید (از منوی سه‌نقطه)
**رشته‌ی ارسالی:** `ASSIGN_TO_SHELF||bookId||newShelfId`
**پاسخ:** `ASSIGN_SHELF_RESULT||SUCCESS`

**نکته:** جایگزینِ کاملِ `ADD_BOOK_TO_SHELF` قدیمیه (که فیلدهاش هم برعکس بود: `shelfId||bookId`) — `ADD_BOOK_TO_SHELF` دیگه استفاده نمی‌شه.

---

### پروتکل دانلود PDF (چندپیامی)

برخلاف بقیه‌ی دستورها که یه request و یه response دارن، دانلود فایل یه **جریان چندپیامی** از سمت سرور به کلاینته:

1. **کلاینت:** `DOWNLOAD_BOOK||bookId`
2. **سرور (اول):** `FILE_START||bookId||totalNumberOfChunks||lastReadPage`
   (`lastReadPage`: آخرین صفحه‌ای که کاربر قبلاً خونده — یعنی سرور موقع شروع دانلود هم موقعیتِ مطالعه رو برمی‌گردونه.)
3. **سرور (تکرار، به تعداد `totalNumberOfChunks`):** `FILE_CHUNK||bookId||base64ChunkString`
4. **سرور (آخر):** `FILE_END||bookId`

بعداً، هروقت کاربر صفحه‌ای که توش هست عوض بشه، کلاینت جدا از این جریان می‌فرسته:

`UPDATE_LAST_PAGE||bookId||pageNumber`

**پاسخش:** `UPDATE_LAST_PAGE||SUCCESS` یا `UPDATE_LAST_PAGE||FAIL` (مثل `LOGIN`/`SIGNUP_*`، با نام خودِ دستور شروع می‌شه، نه `SUCCESS`/`FAIL` تنها؛ حروف بزرگ.)

**همزمانی چند دانلود:** نیازی به یه ID مخصوصِ دانلود نیست — سرور هر دانلود رو تو ترد جدای خودش (مثل هر `ClientHandler`/کانکشن دیگه) هندل می‌کنه، پس دو تا دانلود همزمان (چه از یه کاربر، چه از کاربرهای مختلف) طبیعتاً با هم قاطی نمی‌شن.

**اگه یه `FILE_CHUNK` گم بشه یا خراب برسه:** مکانیزم retry/recovery نداره — کلاینت باید از اول `DOWNLOAD_BOOK` رو دوباره بزنه.

---

### `SAVE_BOOK` 🔒 — افزودن به لیست علاقه‌مندی‌ها
**رشته‌ی ارسالی:** `SAVE_BOOK||bookId`
**پاسخ:** `SUCCESS||"Book saved"`

---

### `GET_COMMENTS_LIST`
**نیاز به لاگین:** ندارد

**رشته‌ی ارسالی:** `GET_COMMENTS_LIST||bookId`

**پاسخ:** `COMMENTS_LIST||commentId1,commentId2`
مثال: `COMMENTS_LIST||C_1,C_2`

⚠️ **نکته‌ی فرمت id:** برخلاف bookId (که عدد ساده‌ست، مثل `1`)، اینجا commentIdها با پیشوند `C_` هستن (مثل `C_1`, `C_2`). این یعنی موقع پردازش دیگه نمی‌شه commentId رو مستقیم به عدد تبدیل کرد؛ باید اول `C_` رو جدا کرد.

این گام «list» از الگوی list→detail برای کامنت‌هاست — گام «detail»ش پایینه.

---

### `GET_COMMENT_DETAILS`
**نیاز به لاگین:** ندارد

**رشته‌ی ارسالی:** `GET_COMMENT_DETAILS||commentId`

**پاسخ:**
```
COMMENT_DETAILS||commentId||userName||fullName||rate||commentString||dateYYYY/MM/DD
```
مثال: `COMMENT_DETAILS||C_1||username||fullname||rate||commentString||dateYYYY/MM/DD`

این گام «detail» بعد از `GET_COMMENTS_LIST` هست (شبیه رابطه‌ی `SEARCH_STOREPAGE`+`GET_BOOK_SUMMARY`).

---

### `UPDATE_COMMENT` 🔒 — افزودن یا ویرایشِ کامنتِ خودِ کاربر (upsert)
**رشته‌ی ارسالی:** `UPDATE_COMMENT||bookId||text||stars`

سرور با `userId` سشن + این `bookId` چک می‌کنه: اگه کاربر از قبل روی این کتاب کامنت داشته باشه، همون کامنت رو با مقادیر جدید (`text`, `stars`) ویرایش می‌کنه؛ اگه نداشته باشه، یه کامنت جدید براش می‌سازه. یعنی یه دستور واحد، به‌جای دو دستور جدای «افزودن» و «ویرایش».

**پاسخ:** `SUCCESS` یا `FAIL` (ساده، بدون `commentId` و بدون پیام اضافه)

⚠️ هنوز مونده: پوش `COMMENT_UPDATED||bookId` فقط موقع ویرایش می‌ره یا موقع افزودنِ اولیه هم می‌ره؟

⚠️ **جایگزینِ کاملِ `ADD_COMMENT` و `EDIT_COMMENT` قدیمی:** این دو تا دیگه استفاده نمی‌شن — `UPDATE_COMMENT` جای هر دوشون رو گرفته.

---

### `DELETE_COMMENT` 🔒 — حذفِ نظرِ خودِ کاربر

**رشته‌ی ارسالی:** `DELETE_COMMENT||bookId`

(نه `commentId`! ورودی `bookId`ه. سرور با استفاده از `userId` سشن (که رو `ClientHandler` نگه داشته) + همین `bookId`، کامنتِ خودِ همین کاربر روی این کتاب رو تو لیست کامنت‌هاش پیدا می‌کنه و حذفش می‌کنه.)

**پاسخ موفق:** `SUCCESS||"Comment deleted"` *(فرض — لطفاً فرمت دقیق پاسخ رو تأیید کن)*
**پاسخ‌های شکست (فرض):** `FAIL||"Comment not found"` (این کاربر روی این کتاب کامنتی نداشته)

**نکته:** دیگه تداخل اسمی با نسخه‌ی Admin نیست — Admin از دستور جدای `DELETE_COMMENT_ADMIN` (بخش ۹، `commentId`-based) استفاده می‌کنه.

---

## ۵) پروفایل کاربر، تاریخچه‌ی خرید و کیف پول

### `GET_DATA_USERPROFILE` 🔒
**رشته‌ی ارسالی:** `GET_DATA_USERPROFILE` (بدون فیلد؛ `userId` از سشن)

**پاسخ:** `USER_PROFILE_INFO`
```
USER_PROFILE_INFO||fullName||username||email||genresList||walletBalance
```
مثال: `USER_PROFILE_INFO||fullName||username||email||generslist(with,)||walletBlance`
(`genresList`: ژانرهای موردعلاقه با `,` جدا شده، از همون ۸ مقدار enum رشته‌ای. `walletBalance`: موجودی کیف پول.)

---

### `GET_HISTORY_IDS` 🔒
**رشته‌ی ارسالی:** `GET_HISTORY_IDS` (بدون فیلد؛ `userId` از سشن)

**پاسخ:** `HISTORY_IDS`
```
HISTORY_IDS||count||id1,id2,id3
```
مثال: `HISTORY_IDS||3||101,102,103`

اگه تاریخچه خالی باشه:
```
HISTORY_IDS||0
```
(توجه: وقتی `count=0`ه، فیلد سوم (لیست idها) اصلاً وجود نداره — نه این‌که خالی باشه. یعنی پارسر باید تعداد فیلدها رو چک کنه، نه فرض کنه همیشه ۳ فیلده.)

این گام «list» از الگوی list→detail برای تاریخچه‌ی خریده — گام «detail»ش `GET_HISTORY_BOOK` هست.

---

### `GET_HISTORY_BOOK` 🔒
**رشته‌ی ارسالی:** `GET_HISTORY_BOOK||bookId`

**پاسخ:** `HISTORY_BOOK_INFO`
```
HISTORY_BOOK_INFO||bookId||title||author||buyPrice||dateOfBuy
```
(`dateOfBuy`: فرمت `YYYY/MM/DD`)

---

### `UPDATE_BALANCE` 🔒
**رشته‌ی ارسالی:** `UPDATE_BALANCE||newBalance`

موجودی کیف پول رو مستقیماً به `newBalance` **ست** می‌کنه (نه افزایشی).

**پاسخ:** `SUCCESS` یا `FAIL` (ساده، بدون پیام اضافه)

**نکته:** تأیید شد که این طراحی عمدیه — هر کاربری اجازه داره موجودی خودش رو مستقیم ست کنه، و قراره همین‌جوری بمونه (بدون مسیر پرداخت واقعی یا کد تأیید).

---

### `UPDATE_PROFILE` 🔒
**رشته‌ی ارسالی:** `UPDATE_PROFILE||currentName||currentEmail||currentGenresStringList`

**پاسخ موفق:** `UPDATE_PROFILE_RESULT||SUCCESS`
**پاسخ‌های شکست:** `UPDATE_PROFILE_RESULT||DUPLICATE_EMAIL`

(مثل `LOGIN`/`SIGNUP_*`، این پاسخ هم با نام خودِ دستور شروع می‌شه، نه `SUCCESS`/`FAIL` ساده.)

---

## ۶) داشبورد آمار ناشر (Publisher Statistics)

معماری این بخش «ID-first»ه: تقریباً همه‌جا اول یه `GET_..._IDS` می‌زنی، بعد برای هرکدوم جزئیات می‌گیری — دقیقاً همون الگوی list→detail بالای سند.

### `GET_PUB_GENERAL_STATS` 🔒 — فقط Publisher
**رشته‌ی ارسالی:** `GET_PUB_GENERAL_STATS` (بدون فیلد؛ `publisherId` از سشن)
**پاسخ:** `PUB_GENERAL_RESULT||totalBooks||totalAvgScore||totalIncome`
مثال: `PUB_GENERAL_RESULT||15||4.2||450000`

---

### `GET_PUB_TOP_BOOKS_IDS` 🔒 — لیست پرفروش‌ترین‌ها (مرحله‌ی اول)
**رشته‌ی ارسالی:** `GET_PUB_TOP_BOOKS_IDS`
**پاسخ:** `PUB_TOP_IDS_RESULT||bookId1,bookId2,...` (حداکثر ۵ آیدی)

---

### `GET_PUB_BOOK_INFO_TOP` 🔒 — جزئیات یه کتاب تو جدول پرفروش‌ترین‌ها (مرحله‌ی دوم)
**رشته‌ی ارسالی:** `GET_PUB_BOOK_INFO_TOP||bookId`
**پاسخ:** `PUB_BOOK_INFO_TOP_RESULT||bookId||name||salesCount||avgScore`

---

### `GET_PUB_LOWEST_BOOKS_IDS` 🔒 — لیست کم‌فروش‌ترین‌ها (مرحله‌ی اول)
**رشته‌ی ارسالی:** `GET_PUB_LOWEST_BOOKS_IDS`
**پاسخ:** `PUB_LOWEST_IDS_RESULT||bookId1,bookId2,...` (حداکثر ۵ آیدی، که تو لیست پرفروش‌ها نباشن)

---

### `GET_PUB_BOOK_INFO_LOWEST` 🔒 — جزئیات یه کتاب تو جدول کم‌فروش‌ترین‌ها (مرحله‌ی دوم)
**رشته‌ی ارسالی:** `GET_PUB_BOOK_INFO_LOWEST||bookId`
**پاسخ:** `PUB_BOOK_INFO_LOWEST_RESULT||bookId||name||salesCount||avgScore`

---

### `GET_PUB_SALES_CHART` 🔒 — نمودار دایره‌ای (سهم از کل فروش)
**رشته‌ی ارسالی:** `GET_PUB_SALES_CHART`
**پاسخ:** `PUB_SALES_CHART_RESULT||bookName1:salesCount1,bookName2:salesCount2,...,سایر کتاب‌ها:salesCountX`

(سرور **تعداد فروش خام** رو می‌فرسته، نه درصد — کلاینت خودش درصد رو حساب می‌کنه و رسم می‌کنه. سرور به ترتیب فروشِ بیشتر، تعدادی کتاب می‌فرسته که مجموعشون حداقل ۷۰٪ کل فروش رو پوشش بده؛ اگه تعداد کل کتاب‌های ناشر ≤ ۵ تا باشه، همه‌شون فرستاده می‌شن. بقیه‌ی فروش زیر یه ردیف «سایر کتاب‌ها» جمع می‌شه.)

---

### `GET_PUB_SCORE_CHART` 🔒 — نمودار میله‌ای (میانگین امتیاز هر کتاب)
**رشته‌ی ارسالی:** `GET_PUB_SCORE_CHART`
**پاسخ:** `PUB_SCORE_CHART_RESULT||bookName1:score1,bookName2:score2,...`
(برخلاف `GET_PUB_SALES_CHART`، اینجا محدودیت ۵تایی یا آستانه‌ی درصدی نیست — همه‌ی کتاب‌ها با هر تعدادی فرستاده می‌شن.)

---

### `GET_PUB_PROFILE_INFO` 🔒 — فقط Publisher
کاربرد: گرفتن اطلاعات اولیه‌ی ناشر موقع ورود به صفحه‌ی پروفایل.

**رشته‌ی ارسالی:** `GET_PUB_PROFILE_INFO` (بدون فیلد؛ `publisherId` از سشن)

**پاسخ:** `PUB_PROFILE_INFO` — تعداد فیلدهاش متغیره (فیلد آخر یعنی bio اختیاریه):

| حالت | فرمت | مثال |
|---|---|---|
| با bio (۵ فیلد بعد از نام دستور) | `PUB_PROFILE_INFO\|\|publisherName\|\|username\|\|email\|\|balance\|\|bio` | `PUB_PROFILE_INFO\|\|نشر چشمه\|\|cheshmeh_admin\|\|info@cheshmeh.ir\|\|450000\|\|بهترین ناشر ایران` |
| بدون bio (۴ فیلد) | `PUB_PROFILE_INFO\|\|publisherName\|\|username\|\|email\|\|balance` | `PUB_PROFILE_INFO\|\|نشر چشمه\|\|cheshmeh_admin\|\|info@cheshmeh.ir\|\|450000` |
| بدون bio، با `\|\|` خالی انتهایی | `PUB_PROFILE_INFO\|\|publisherName\|\|username\|\|email\|\|balance\|\|` | `PUB_PROFILE_INFO\|\|نشر چشمه\|\|cheshmeh_admin\|\|info@cheshmeh.ir\|\|450000\|\|` |

⚠️ **نکته‌ی مهم برای پیاده‌سازی پارسر:** برخلاف بقیه‌ی سند که تعداد فیلدها ثابته، اینجا صراحتاً ۳ حالت متفاوت (۵ فیلد، ۴ فیلد، یا ۵ فیلد با آخری خالی) مجازه. پارسر باید هر سه‌تا رو قبول کنه، نه فقط یکی.

---

### `UPDATE_PUB_PROFILE` 🔒 — فقط Publisher
**رشته‌ی ارسالی:** `UPDATE_PUB_PROFILE||newPublisherName||newEmail||newBio`

**پاسخ موفق:** `UPDATE_PUB_PROFILE_RESULT||SUCCESS`

**پاسخ‌های شکست:**
```
UPDATE_PUB_PROFILE_RESULT||DUPLICATE_EMAIL
UPDATE_PUB_PROFILE_RESULT||DUPLICATE_ENTESHARAT
```
(`DUPLICATE_ENTESHARAT`: نام انتشارات/ناشر تکراریه. **تأیید شد:** درست مثل `SIGNUP_PUBLISHER`/`SIGNUP_NORMALUSER`، اگه هر دو هم‌زمان تکراری باشن، فقط **یکی** از این دو کد برمی‌گرده، هیچ‌وقت هر دو با هم.)

---

### `WITHDRAW_BALANCE` 🔒 — فقط Publisher — برداشت/تسویه‌حساب
**رشته‌ی ارسالی:** `WITHDRAW_BALANCE` (بدون فیلد؛ `publisherId` از سشن)

**پاسخ موفق:** `WITHDRAW_BALANCE_RESULT||SUCCESS`
**پاسخ شکست:** `WITHDRAW_BALANCE_RESULT||FAIL` (بدون علتِ جدا — همین یه کلمه کافیه)

**تأیید شد:** همیشه **کل موجودی** ناشر برداشت می‌شه (بدون امکان انتخاب مبلغ جزئی) — به همین خاطر رشته‌ی ارسالی فیلدی نداره.

---

## ۷) مدیریت کاربران (فقط Admin)

معماری این بخش هم «ID-first»ه: اول `SEARCH_USERS` لیست idها رو می‌ده، بعد برای هرکدوم با `GET_USER_INFO` جزئیات می‌گیری.

### `SEARCH_USERS` 🔒 — فقط Admin
**رشته‌ی ارسالی:** `SEARCH_USERS||name||role||status`

- `name`: متنی که ادمین تایپ کرده؛ اگه خالی بذاره، کلاینت دقیقاً کلمه‌ی `EMPTY` می‌فرسته.
- `role`: یکی از `ALL` / `PUBLISHER` / `NORMAL`
- `status`: یکی از `ALL` / `UNBLOCK` (فعال) / `BLOCK` (مسدود)

مثال با فیلتر: `SEARCH_USERS||ali||NORMAL||UNBLOCK`
مثال بدون فیلتر: `SEARCH_USERS||EMPTY||ALL||ALL`

**پاسخ:** `USERS_LIST_RESULT||count||idList`
مثال با نتیجه: `USERS_LIST_RESULT||2||1001,1002`
مثال بدون نتیجه: `USERS_LIST_RESULT||0` (توجه: وقتی count=0ه، فیلد سوم اصلاً نیست — نه خالی، دقیقاً مثل `HISTORY_IDS||0`.)

این گام «list»ه؛ گام «detail»ش `GET_USER_INFO` هست.

---

### `GET_USER_INFO` 🔒 — فقط Admin
**رشته‌ی ارسالی:** `GET_USER_INFO||userId`

**پاسخ:** `USER_INFO_RESULT||userId||displayName||username||role||email||regDate||status`
مثال: `USER_INFO_RESULT||1001||نشر چشمه||cheshmeh_pub||PUBLISHER||info@cheshmeh.ir||2023/01/10||UNBLOCK`

(`role`: `PUBLISHER` یا `NORMAL`. `status`: `UNBLOCK` یا `BLOCK`. `regDate`: فرمت `YYYY/MM/DD`. همین یه دستور هم برای کاربر عادی هم برای ناشر جواب می‌ده — تفکیک از رو فیلد `role`ه.)

---

### `DELETE_USER` 🔒 — فقط Admin — حذف دائم حساب
**رشته‌ی ارسالی:** `DELETE_USER||userId`
**پاسخ:** `DELETE_USER_RESULT||userId||SUCCESS` یا `DELETE_USER_RESULT||userId||FAILED`

(این با `BLOCK_USER` فرق داره — دائمیه، نه موقت.)

---

### `BLOCK_USER` 🔒 — فقط Admin
**رشته‌ی ارسالی:** `BLOCK_USER||userId`
**پاسخ:** `BLOCK_USER_RESULT||userId||SUCCESS` یا `BLOCK_USER_RESULT||userId||FAILED`

⚠️ **جایگزینِ فرمت پاسخ قدیمی:** قبلاً پاسخش `SUCCESS||"User blocked"` / `FAIL||"Not authorized"` بود. الان با فرمت `BLOCK_USER_RESULT||userId||SUCCESS/FAILED` جایگزین شد (خودنام، شامل `userId`، بدون پیام متنی جدا برای علت شکست).

**نکته:** اگه هدف آنلاین باشه، سشنش هنوز قطع می‌شه (رفتار قبلی حفظ شده).

---

### `UNBLOCK_USER` 🔒 — فقط Admin
**رشته‌ی ارسالی:** `UNBLOCK_USER||userId`
**پاسخ:** `UNBLOCK_USER_RESULT||userId||SUCCESS` یا `UNBLOCK_USER_RESULT||userId||FAILED`

⚠️ همون تغییر فرمتِ `BLOCK_USER` اینجا هم صادقه.

---

## ۸) مدیریت کتاب‌ها و کامنت‌ها (فقط Admin)

✅ **برخوردهای اسم قبلی، حل شدن** (با فرستادن نسخه‌ی اصلاح‌شده):
1. `DELETE_BOOK` — همون فرمت جدید بخش ۲ (`DELETE_BOOK_RESULT||bookId||SUCCESS/FAILED`)، تداخلی نیست.
2. `EDIT_BOOK` نسخه‌ی Admin → اسمش شد `EDIT_BOOK_ADMIN` (جدا از `EDIT_BOOK` قدیمیِ Publisher تو بخش ۲).
3. `DELETE_COMMENT` نسخه‌ی Admin → اسمش شد `DELETE_COMMENT_ADMIN` (جدا از `DELETE_COMMENT` خودِ کاربر تو بخش ۴).

### `GET_ALL_BOOKS_ADMIN` 🔒 — فقط Admin — لیست تمام کتاب‌ها بدون فیلتر
**رشته‌ی ارسالی:** `GET_ALL_BOOKS_ADMIN` (بدون فیلد)
**پاسخ:** `ALL_BOOKS_ADMIN_RESULT||count||idList`
مثال: `ALL_BOOKS_ADMIN_RESULT||2||201,202`
(اگه کتابی نبود: `ALL_BOOKS_ADMIN_RESULT||0`)

---

### `GET_BOOK_INFO_ADMIN` 🔒 — فقط Admin
**رشته‌ی ارسالی:** `GET_BOOK_INFO_ADMIN||bookId`
**پاسخ:** `BOOK_INFO_ADMIN_RESULT||bookId||bookName||authorName||publisherName||genreEn||description||status`

- `genreEn`: یکی از `ROMANCE, CRIME, SCIFI, HORROR, CLASSIC, ART, COMEDY, HISTORY` (تأیید شد: casing واقعی همین همه‌حروف‌بزرگه — enum اصلی سند هم به همین صورت اصلاح شد.)
- `description`: اگه خالی بود، بین دو `||` هیچی نمیاد (یعنی `||||` تو وسط پیام).
- `status`: `ACTIVE` یا `INACTIVE`.

مثال با توضیح: `BOOK_INFO_ADMIN_RESULT||201||بوف کور||صادق هدایت||نشر الف||CLASSIC||یک رمان عالی||ACTIVE`
مثال بدون توضیح: `BOOK_INFO_ADMIN_RESULT||202||کتاب جدید||علی||نشر ب||ART||||INACTIVE`

---

### حذف کتاب — ✅ حل شد، همون `DELETE_BOOK` بخش ۲ست
دیگه تداخلی نیست — `DELETE_BOOK` (بخش ۲) الان با همین فرمت جدید (`DELETE_BOOK_RESULT||bookId||SUCCESS/FAILED`) نهایی شده.

---

### `EDIT_BOOK_ADMIN` 🔒 — فقط Admin — ویرایش کتاب (نسخه‌ی Admin)
```
EDIT_BOOK_ADMIN||bookId||newName||newAuthor||newGenreEn||newDesc
```
**نکته:** چون `newDesc` آخرین فیلده، اگه ادمین توضیحات رو خالی بذاره، پیام مستقیماً به یه `||` ختم می‌شه و بعدش هیچی نیست.
مثال: `EDIT_BOOK_ADMIN||201||بوف کور||صادق هدایت||CLASSIC||`

**پاسخ موفق:** سرور دقیقاً همون مقادیرِ ثبت‌شده رو با همون ترتیب برمی‌گردونه (تا کلاینت بدون رفرش کل جدول، فقط همون ردیف رو آپدیت کنه):
```
EDIT_BOOK_ADMIN_RESULT||bookId||SUCCESS||newName||newAuthor||newGenreEn||newDesc
```
مثال: `EDIT_BOOK_ADMIN_RESULT||201||SUCCESS||بوف کور||صادق هدایت||CLASSIC||`

**پاسخ شکست:** `EDIT_BOOK_ADMIN_RESULT||bookId||FAILED`

⚠️ فیلدهاش با `EDIT_BOOK` قدیمیِ Publisher (بخش ۲) کاملاً متفاوته (نه قیمت/تخفیف/عکس داره، فقط نام/نویسنده/ژانر/توضیحات) — این دو تا دستورِ جدا و مستقلن، فقط کاربردشون شبیه همه.

---

### `GET_BOOK_COVER` 🔒 — فقط Admin — گرفتن فقط عکس جلد
**رشته‌ی ارسالی:** `GET_BOOK_COVER||bookId`
**پاسخ:** `BOOK_COVER_RESULT||bookId||base64String`

---

### دانلود PDF (نسخه‌ی Admin، چندپیامی)
```
DOWNLOAD_PDF_ADMIN||bookId
```
پاسخ سرور، سه‌مرحله‌ای:
```
PDF_START||bookId||totalChunks
PDF_CHUNK||bookId||base64Data      (تکراری، به تعداد totalChunks)
PDF_END||bookId||SUCCESS
```
(هر سه پیام `bookId` دارن — مثل پروتکل دانلود کاربر عادی تو بخش ۴.)

---

## ۹) مدیریت نظرات (فقط Admin)

### `GET_ALL_COMMENTS_ADMIN` 🔒 — فقط Admin
**رشته‌ی ارسالی:** `GET_ALL_COMMENTS_ADMIN` (بدون فیلد)
**پاسخ:** `ALL_COMMENTS_ADMIN_RESULT||count||commentIdList`
(اگه نظری نبود: `ALL_COMMENTS_ADMIN_RESULT||0`)

---

### `GET_COMMENT_INFO` 🔒 — فقط Admin
**رشته‌ی ارسالی:** `GET_COMMENT_INFO||commentId`
**پاسخ:** `COMMENT_INFO_RESULT||commentId||userName||bookName||commentText||date`
(`commentText`: اگه خالی بود، بین دو `||` هیچی نمیاد.)

این با `GET_COMMENT_DETAILS`/`COMMENT_DETAILS` بخش ۴ (که `userName`+`fullName`+`rate` می‌داد، بدون `bookName`) فرق داره — اسم جداست پس تداخلی نیست، فقط دو تا view متفاوت از یه چیزن (یکی برای کاربر عادی موقع دیدنِ کامنت‌های یه کتاب، یکی برای Admin موقع مدیریت کلی کامنت‌ها).

---

### `DELETE_COMMENT_ADMIN` 🔒 — فقط Admin
```
DELETE_COMMENT_ADMIN||commentId
```
**پاسخ:** `DELETE_COMMENT_ADMIN_RESULT||commentId||SUCCESS` یا `DELETE_COMMENT_ADMIN_RESULT||commentId||FAILED`

این جدا از `DELETE_COMMENT` بخش ۴ (خودِ کاربر، `bookId`-based، حذفِ کامنتِ خودش) هست — دو تا دستور مستقل، بدون تداخل اسم.

---

## ۱۰) اعلان‌ها و آمار زنده (مشترک بین همه‌ی نقش‌ها + اختصاصی Admin)

نکته‌ی معماری: بخش «اعلان‌ها» برای هر سه نقش (Admin، Publisher، NormalUser) دقیقاً یه ساختاره؛ سرور کاربر رو از رو `userId` سشن تفکیک می‌کنه (نه از رو نقش). فقط «آمار سیستم» (`UPDATE_ADMIN_STATS`) مخصوص سوکتِ Adminه.

**چرا دو جریان جدا (تاریخچه/list + زنده/push)؟** تا موقع ورود کاربر به برنامه، اگه چندتا اعلان قدیمی/آفلاین داشته باشه، همه‌شون یه‌جا بوق نزنن و نوار وضعیت رو پر نکنن — `GET_NOTIFICATIONS` (زیر) فقط بی‌صدا لیست زنگوله رو پر می‌کنه، و فقط `NEW_NOTIFICATION_PUSH` (بخش پوش‌های خودکار، انتهای سند) که واقعاً لحظه‌ایه بوق می‌زنه.

### `GET_NOTIFICATIONS` 🔒 — گام «list»، فقط تاریخچه/آفلاین
**رشته‌ی ارسالی:** `GET_NOTIFICATIONS` (بدون فیلد؛ `userId` از سشن)
**پاسخ:** `NOTIFICATIONS_RESULT||count||id1,id2,id3`
مثال: `NOTIFICATIONS_RESULT||3||101,102,103`

این پیام‌ها فقط باید بی‌صدا تو لیست زنگوله بشینن — نه بوق بزنن نه تو نوار وضعیت نشون داده بشن (چون تاریخچه‌ان، نه لحظه‌ای).

---

### `GET_NOTIFICATION_INFO` 🔒 — گام «detail»
**رشته‌ی ارسالی:** `GET_NOTIFICATION_INFO||notifId`
**پاسخ:** `NOTIFICATION_INFO_RESULT||notifId||message||date||status`
(`status`: `UNREAD` یا `READ`.)
مثال: `NOTIFICATION_INFO_RESULT||101||ناشر جدیدی ثبت‌نام کرد.||2026/07/25||UNREAD`

**تأیید شد:** فرمت تاریخ همه‌جای سند میلادیه (`YYYY/MM/DD`) — مثال قبلی این بخش که به اشتباه تقویم شمسی داشت (`1405/05/12`) اصلاح شد.

---

### `MARK_NOTIFICATION_READ` 🔒
**رشته‌ی ارسالی:** `MARK_NOTIFICATION_READ||notifId`
**پاسخ:** `MARK_READ_RESULT||notifId||SUCCESS` یا `MARK_READ_RESULT||notifId||FAILED`

---

### `MARK_ALL_NOTIFICATIONS_READ` 🔒
**رشته‌ی ارسالی:** `MARK_ALL_NOTIFICATIONS_READ` (بدون فیلد)
**پاسخ:** `MARK_ALL_READ_RESULT||SUCCESS` یا `MARK_ALL_READ_RESULT||FAILED`

---

## ۱۱) پوش‌های خودکار سرور (بدون درخواست کلاینت)

| نام | فرمت | چه‌موقع |
|---|---|---|
| `COMMENT_UPDATED` | `COMMENT_UPDATED\|\|bookId` | بعد از هر `UPDATE_COMMENT` (احتمالاً فقط حالتِ ویرایش، نه افزودنِ اولیه — نیاز به تأیید)، به همه‌ی کلاینت‌های آنلاین |
| `UPDATE_ADMIN_STATS` | `UPDATE_ADMIN_STATS\|\|onlineUsers\|\|totalUsers` | فقط برای سوکتِ Admin؛ هر بار یه کاربر لاگین کنه، لاگ‌اوت کنه، کانکشنش قطع بشه، یا ثبت‌نام کنه. مثال: `UPDATE_ADMIN_STATS\|\|16\|\|240` |
| `NEW_NOTIFICATION_PUSH` | `NEW_NOTIFICATION_PUSH\|\|notifId\|\|message\|\|date\|\|UNREAD` | لحظه‌ای، وقتی کاربر آنلاینه و یه اتفاق جدید (مثلاً کاربر جدید، فروش جدید) رخ می‌ده — کلاینت باید بوق بزنه و تو نوار وضعیت نشون بده. مثال: `NEW_NOTIFICATION_PUSH\|\|104\|\|کاربر جدید وارد شد.\|\|2026/07/25 - 18:30\|\|UNREAD` (توجه: اینجا فرمت تاریخ ساعت هم داره، برخلاف `GET_NOTIFICATION_INFO` که فقط تاریخه؛ تاریخ میلادیه.) |
| نوتیفیکیشن داخلی (عمومی) | پیام متنی | کتاب جدید در ژانر موردعلاقه، فروش برای ناشر، تکمیل خرید — این‌ها همون چیزیه که از رو `GET_NOTIFICATIONS`/`NEW_NOTIFICATION_PUSH` بالا میان، نه یه مکانیزم جدا. |

**تأیید شد:** پاک‌سازی زمان‌محور نیست — سرور اعلانی رو که کاربر دیده (خونده/`READ` شده) همون لحظه از دیتابیس پاک می‌کنه، نه بعد از یه بازه‌ی زمانی مشخص. یعنی این پاک‌سازی احتمالاً همون لحظه‌ی `MARK_NOTIFICATION_READ`/`MARK_ALL_NOTIFICATIONS_READ` انجام می‌شه، نه یه job جدا و زمان‌بندی‌شده.

---

## TODO — دستورهای هنوز پیاده‌نشده (بخش ادمین)

فرمت دقیق `CMD||...` و پاسخ سرورشون هنوز مشخص نشده؛ وقتی پیاده شدن باید مثل بقیه‌ی سند کامل بشن:

(همه‌ی موارد قبلی این لیست پوشش داده شدن — بخش ۸ و ۹ رو ببین. برخوردهای اسمی هم حل شدن؛ این بخش دیگه چیزی نداره.)

---

## جدول خلاصه‌ی دستورها

| نام دستور | نیاز به لاگین |
|---|---|
| LOGIN | خیر |
| SIGNUP_PUBLISHER | خیر |
| SIGNUP_NORMALUSER | خیر |
| FORGOT_PASS_CHECK | خیر |
| FORGOT_PASS_UPDATE | خیر (وابسته به سشن قبلی) |
| LOGOUT | بله |
| GET_PUBLISHER_BOOKS | بله (Publisher) |
| GET_PUB_BOOK_DETAILS | بله (Publisher/Admin — نه کاربر عادی) |
| ADD_BOOK_METADATA | بله (Publisher) |
| UPLOAD_PDF_START/CHUNK/END | بله (Publisher) |
| EDIT_BOOK | بله (ناشر صاحب کتاب) |
| TOGGLE_BOOK_STATUS | بله (ناشر صاحب کتاب) |
| DELETE_BOOK | بله (Admin) |
| SEARCH_STOREPAGE | فقط برای display=recommended |
| GET_BOOK_SUMMARY | خیر |
| GET_BOOK_DETAILS | بله |
| GET_COMMENTS_LIST | خیر |
| GET_COMMENT_DETAILS | خیر |
| BUY | بله |
| GET_CART_ITEMS | بله |
| GET_CART_BOOK_SUMMARY | بله |
| ADD_CART | بله |
| REMOVE_CART | بله |
| CHECKOUT_CART | بله |
| GET_MY_BOOKS | بله |
| GET_SAVED_BOOKS | بله |
| GET_SHELVES | بله |
| GET_SHELF_BOOKS | بله |
| GET_MY_BOOK_INFO | بله |
| GET_SAVED_BOOK_INFO | بله |
| GET_SHELF_BOOK_INFO | بله |
| ADD_SHELF | بله |
| EDIT_SHELF | بله |
| DELETE_SHELF | بله |
| UNSAVE_BOOK | بله |
| REMOVE_FROM_SHELF | بله |
| ASSIGN_TO_SHELF | بله |
| DOWNLOAD_BOOK | بله |
| UPDATE_LAST_PAGE | بله |
| SAVE_BOOK | بله |
| UPDATE_COMMENT | بله |
| DELETE_COMMENT | بله (صاحب کامنت) |
| GET_DATA_USERPROFILE | بله |
| GET_HISTORY_IDS | بله |
| GET_HISTORY_BOOK | بله |
| UPDATE_BALANCE | بله (ست مستقیم، عمداً بدون محدودیت) |
| UPDATE_PROFILE | بله |
| GET_PUB_GENERAL_STATS | بله (Publisher) |
| GET_PUB_TOP_BOOKS_IDS | بله (Publisher) |
| GET_PUB_BOOK_INFO_TOP | بله (Publisher) |
| GET_PUB_LOWEST_BOOKS_IDS | بله (Publisher) |
| GET_PUB_BOOK_INFO_LOWEST | بله (Publisher) |
| GET_PUB_SALES_CHART | بله (Publisher) |
| GET_PUB_SCORE_CHART | بله (Publisher) |
| GET_PUB_PROFILE_INFO | بله (Publisher) |
| UPDATE_PUB_PROFILE | بله (Publisher) |
| WITHDRAW_BALANCE | بله (Publisher) |
| SEARCH_USERS | بله (Admin) |
| GET_USER_INFO | بله (Admin) |
| DELETE_USER | بله (Admin) |
| GET_ALL_BOOKS_ADMIN | بله (Admin) |
| GET_BOOK_INFO_ADMIN | بله (Admin) |
| DELETE_BOOK | بله (Admin) — یکی شد، بخش ۲ رو ببین |
| EDIT_BOOK_ADMIN | بله (Admin) |
| GET_BOOK_COVER | بله (Admin) |
| DOWNLOAD_PDF_ADMIN | بله (Admin) |
| GET_ALL_COMMENTS_ADMIN | بله (Admin) |
| GET_COMMENT_INFO | بله (Admin) |
| DELETE_COMMENT_ADMIN | بله (Admin) |
| BLOCK_USER | بله (Admin) |
| UNBLOCK_USER | بله (Admin) |
| GET_NOTIFICATIONS | بله |
| GET_NOTIFICATION_INFO | بله |
| MARK_NOTIFICATION_READ | بله |
| MARK_ALL_NOTIFICATIONS_READ | بله |

| نام پاسخ / نوتیفیکیشن |
|---|
| SUCCESS |
| FAIL |
| SEARCH_RESULT |
| BOOK_SUMMARY |
| BOOK_DETAILS_RESULT |
| PUBLISHER_BOOKS_RESULT |
| PUB_BOOK_DETAILS_RESULT |
| ADD_BOOK_RESULT |
| UPLOAD_PDF_RESULT |
| EDIT_BOOK_RESULT |
| TOGGLE_STATUS_RESULT |
| CART_RESULT |
| CART_BOOK_SUMMARY |
| CHECKOUT_RESULT |
| USER_PROFILE_INFO |
| HISTORY_IDS |
| HISTORY_BOOK_INFO |
| UPDATE_PROFILE_RESULT |
| PUB_GENERAL_RESULT |
| PUB_TOP_IDS_RESULT |
| PUB_BOOK_INFO_TOP_RESULT |
| PUB_LOWEST_IDS_RESULT |
| PUB_BOOK_INFO_LOWEST_RESULT |
| PUB_SALES_CHART_RESULT |
| PUB_SCORE_CHART_RESULT |
| PUB_PROFILE_INFO |
| UPDATE_PUB_PROFILE_RESULT |
| WITHDRAW_BALANCE_RESULT |
| USERS_LIST_RESULT |
| USER_INFO_RESULT |
| DELETE_USER_RESULT |
| BLOCK_USER_RESULT |
| UNBLOCK_USER_RESULT |
| ALL_BOOKS_ADMIN_RESULT |
| BOOK_INFO_ADMIN_RESULT |
| DELETE_BOOK_RESULT |
| EDIT_BOOK_ADMIN_RESULT |
| BOOK_COVER_RESULT |
| ALL_COMMENTS_ADMIN_RESULT |
| COMMENT_INFO_RESULT |
| DELETE_COMMENT_ADMIN_RESULT |
| MY_BOOKS_RESULT |
| SAVED_BOOKS_RESULT |
| SHELVES_RESULT |
| SHELF_BOOKS_RESULT |
| MY_BOOK_INFO_RESULT |
| SAVED_BOOK_INFO_RESULT |
| SHELF_BOOK_INFO_RESULT |
| ADD_SHELF_RESULT |
| EDIT_SHELF_RESULT |
| DELETE_SHELF_RESULT |
| REMOVE_SAVED_RESULT |
| REMOVE_FROM_SHELF_RESULT |
| ASSIGN_SHELF_RESULT |
| FILE_START (پوش) |
| FILE_CHUNK (پوش) |
| FILE_END (پوش) |
| COMMENTS_LIST |
| COMMENT_DETAILS |
| COMMENT_UPDATED (پوش) |
| NOTIFICATIONS_RESULT |
| NOTIFICATION_INFO_RESULT |
| MARK_READ_RESULT |
| MARK_ALL_READ_RESULT |
| NEW_NOTIFICATION_PUSH (پوش) |
| UPDATE_ADMIN_STATS (پوش) |
| PDF_START (پوش، نسخه‌ی Admin) |
| PDF_CHUNK (پوش، نسخه‌ی Admin) |
| PDF_END (پوش، نسخه‌ی Admin) |

**نکته:** پاسخ‌های `LOGIN`، `SIGNUP_PUBLISHER`، `SIGNUP_NORMALUSER`، `FORGOT_PASS_CHECK`، `FORGOT_PASS_UPDATE`، `CHECKOUT_CART`، `UPDATE_PROFILE` و `UPDATE_LAST_PAGE` استثنا هستن و با نام خودِ دستور یا یه نام مخصوص (مثل `CHECKOUT_RESULT`, `UPDATE_PROFILE_RESULT`) شروع می‌شن، نه با `SUCCESS`/`FAIL` تنها.

---

## چک‌لیست تصمیم‌های باز (باید قبل از پیاده‌سازی کد واقعی جواب داده بشن)

1. **`UPDATE_COMMENT`:** پاسخش ساده‌ی `SUCCESS`/`FAIL`ه (تأیید شد، بدون `commentId`). هنوز مونده: پوش `COMMENT_UPDATED` فقط موقع ویرایش می‌ره یا موقع افزودنِ اولیه هم؟

---

## نقشه‌ی خطوط سند — همه‌ی درخواست‌ها و پاسخ‌های متناظرشون

ستون آخر شماره‌خطیه که فرمت تفصیلیِ کامل همون دستور، تو همین فایل شروع می‌شه.

### ۱) احراز هویت و حساب کاربری

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `LOGIN` | `LOGIN\|\|SUCCESS/FAIL` | ۴۰ |
| `SIGNUP_PUBLISHER` | `SIGNUP_PUBLISHER\|\|SUCCESS/FAIL` | ۶۵ |
| `SIGNUP_NORMALUSER` | `SIGNUP_NORMALUSER\|\|SUCCESS/FAIL` | ۸۴ |
| `FORGOT_PASS_CHECK` | `FORGOT_PASS_CHECK\|\|SUCCESS/FAIL` | ۱۰۴ |
| `FORGOT_PASS_UPDATE` | `FORGOT_PASS_UPDATE\|\|SUCCESS/FAIL` | ۱۱۵ |
| `LOGOUT` | `SUCCESS`/`FAIL` | ۱۳۱ |

### ۲) کتاب و ناشر

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `GET_PUBLISHER_BOOKS` | `PUBLISHER_BOOKS_RESULT` | ۱۳۹ |
| `GET_PUB_BOOK_DETAILS` | `PUB_BOOK_DETAILS_RESULT` | ۱۴۹ |
| `ADD_BOOK_METADATA` | `ADD_BOOK_RESULT` | ۱۶۴ |
| `UPLOAD_PDF_START`/`UPLOAD_PDF_CHUNK`/`UPLOAD_PDF_END` | `UPLOAD_PDF_RESULT` | ۱۶۴ (فاز ۲، همون بخش) |
| `EDIT_BOOK` | `EDIT_BOOK_RESULT` | ۱۸۸ |
| `TOGGLE_BOOK_STATUS` | `TOGGLE_STATUS_RESULT` | ۲۰۰ |
| `DELETE_BOOK` | `DELETE_BOOK_RESULT` | ۲۱۱ |
| `SEARCH_STOREPAGE` | `SEARCH_RESULT` | ۲۱۹ |
| `GET_BOOK_SUMMARY` | `BOOK_SUMMARY` | ۲۶۹ |
| `GET_BOOK_DETAILS` (کاربر عادی) | `BOOK_DETAILS_RESULT` | ۲۸۶ |

### ۳) خرید و سبد خرید

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `BUY` | `SUCCESS`/`FAIL` | ۳۰۵ |
| `GET_CART_ITEMS` | `CART_RESULT` | ۳۱۲ |
| `GET_CART_BOOK_SUMMARY` | `CART_BOOK_SUMMARY` | ۳۲۷ |
| `ADD_CART` | `SUCCESS`/`FAIL` | ۳۴۰ |
| `REMOVE_CART` | `SUCCESS`/`FAIL` | ۳۴۶ |
| `CHECKOUT_CART` | `CHECKOUT_RESULT` | ۳۵۲ |

### ۴) کتابخانه، قفسه‌ها، دانلود، کامنت‌ها

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `GET_MY_BOOKS` | `MY_BOOKS_RESULT` | ۳۶۴ |
| `GET_SAVED_BOOKS` | `SAVED_BOOKS_RESULT` | ۳۷۲ |
| `GET_SHELVES` | `SHELVES_RESULT` | ۳۸۰ |
| `GET_SHELF_BOOKS` | `SHELF_BOOKS_RESULT` | ۳۸۷ |
| `GET_MY_BOOK_INFO` | `MY_BOOK_INFO_RESULT` | ۳۹۶ |
| `GET_SAVED_BOOK_INFO` | `SAVED_BOOK_INFO_RESULT` | ۳۹۶ |
| `GET_SHELF_BOOK_INFO` | `SHELF_BOOK_INFO_RESULT` | ۳۹۶ |
| `ADD_SHELF` | `ADD_SHELF_RESULT` | ۴۱۴ |
| `EDIT_SHELF` | `EDIT_SHELF_RESULT` | ۴۲۲ |
| `DELETE_SHELF` | `DELETE_SHELF_RESULT` | ۴۲۸ |
| `UNSAVE_BOOK` | `REMOVE_SAVED_RESULT` | ۴۳۴ |
| `REMOVE_FROM_SHELF` | `REMOVE_FROM_SHELF_RESULT` | ۴۴۲ |
| `ASSIGN_TO_SHELF` | `ASSIGN_SHELF_RESULT` | ۴۴۸ |
| `DOWNLOAD_BOOK` | `FILE_START`/`FILE_CHUNK`/`FILE_END` (پوش) | ۴۵۶ |
| `UPDATE_LAST_PAGE` | `UPDATE_LAST_PAGE\|\|SUCCESS/FAIL` | ۴۵۶ (همون بخش) |
| `SAVE_BOOK` | `SUCCESS`/`FAIL` | ۴۷۸ |
| `GET_COMMENTS_LIST` | `COMMENTS_LIST` | ۴۸۴ |
| `GET_COMMENT_DETAILS` | `COMMENT_DETAILS` | ۴۹۸ |
| `UPDATE_COMMENT` | `SUCCESS`/`FAIL` | ۵۱۳ |
| `DELETE_COMMENT` | `SUCCESS`/`FAIL` (فرض) | ۵۲۶ |

### ۵) پروفایل کاربر، تاریخچه، کیف پول

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `GET_DATA_USERPROFILE` | `USER_PROFILE_INFO` | ۵۴۱ |
| `GET_HISTORY_IDS` | `HISTORY_IDS` | ۵۵۳ |
| `GET_HISTORY_BOOK` | `HISTORY_BOOK_INFO` | ۵۷۲ |
| `UPDATE_BALANCE` | `SUCCESS`/`FAIL` | ۵۸۳ |
| `UPDATE_PROFILE` | `UPDATE_PROFILE_RESULT` | ۵۹۴ |

### ۶) داشبورد آمار و پروفایل ناشر

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `GET_PUB_GENERAL_STATS` | `PUB_GENERAL_RESULT` | ۶۰۸ |
| `GET_PUB_TOP_BOOKS_IDS` | `PUB_TOP_IDS_RESULT` | ۶۱۵ |
| `GET_PUB_BOOK_INFO_TOP` | `PUB_BOOK_INFO_TOP_RESULT` | ۶۲۱ |
| `GET_PUB_LOWEST_BOOKS_IDS` | `PUB_LOWEST_IDS_RESULT` | ۶۲۷ |
| `GET_PUB_BOOK_INFO_LOWEST` | `PUB_BOOK_INFO_LOWEST_RESULT` | ۶۳۳ |
| `GET_PUB_SALES_CHART` | `PUB_SALES_CHART_RESULT` | ۶۳۹ |
| `GET_PUB_SCORE_CHART` | `PUB_SCORE_CHART_RESULT` | ۶۴۷ |
| `GET_PUB_PROFILE_INFO` | `PUB_PROFILE_INFO` | ۶۵۴ |
| `UPDATE_PUB_PROFILE` | `UPDATE_PUB_PROFILE_RESULT` | ۶۷۱ |
| `WITHDRAW_BALANCE` | `WITHDRAW_BALANCE_RESULT` | ۶۸۵ |

### ۷) مدیریت کاربران (Admin)

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `SEARCH_USERS` | `USERS_LIST_RESULT` | ۶۹۹ |
| `GET_USER_INFO` | `USER_INFO_RESULT` | ۷۱۷ |
| `DELETE_USER` | `DELETE_USER_RESULT` | ۷۲۷ |
| `BLOCK_USER` | `BLOCK_USER_RESULT` | ۷۳۵ |
| `UNBLOCK_USER` | `UNBLOCK_USER_RESULT` | ۷۴۵ |

### ۸) مدیریت کتاب‌ها و محتوا (Admin)

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `GET_ALL_BOOKS_ADMIN` | `ALL_BOOKS_ADMIN_RESULT` | ۷۶۰ |
| `GET_BOOK_INFO_ADMIN` | `BOOK_INFO_ADMIN_RESULT` | ۷۶۸ |
| (حذف کتاب) | ← همون `DELETE_BOOK` بخش ۲ | ۷۸۱ |
| `EDIT_BOOK_ADMIN` | `EDIT_BOOK_ADMIN_RESULT` | ۷۸۶ |
| `GET_BOOK_COVER` | `BOOK_COVER_RESULT` | ۸۰۵ |
| `DOWNLOAD_PDF_ADMIN` | `PDF_START`/`PDF_CHUNK`/`PDF_END` (پوش) | ۸۱۱ |

### ۹) مدیریت نظرات (Admin)

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `GET_ALL_COMMENTS_ADMIN` | `ALL_COMMENTS_ADMIN_RESULT` | ۸۲۷ |
| `GET_COMMENT_INFO` | `COMMENT_INFO_RESULT` | ۸۳۴ |
| `DELETE_COMMENT_ADMIN` | `DELETE_COMMENT_ADMIN_RESULT` | ۸۴۳ |

### اعلان‌ها و آمار زنده

| درخواست | پاسخ | خط شروع |
|---|---|---|
| `GET_NOTIFICATIONS` | `NOTIFICATIONS_RESULT` | ۸۵۹ |
| `GET_NOTIFICATION_INFO` | `NOTIFICATION_INFO_RESULT` | ۸۶۸ |
| `MARK_NOTIFICATION_READ` | `MARK_READ_RESULT` | ۸۷۸ |
| `MARK_ALL_NOTIFICATIONS_READ` | `MARK_ALL_READ_RESULT` | ۸۸۴ |
| (بدون درخواست — پوش خودکار) | `UPDATE_ADMIN_STATS` | ۸۹۵ |
| (بدون درخواست — پوش خودکار) | `NEW_NOTIFICATION_PUSH` | ۸۹۶ |

⚠️ **توجه:** چون این جدول شماره‌خط داره، هر ویرایش بعدی سند (اضافه/کم‌شدن خط) این شماره‌ها رو جابه‌جا می‌کنه — این جدول باید هر بار که سند structurally عوض می‌شه، دوباره ساخته بشه، وگرنه شماره‌خط‌ها منقضی می‌شن.
