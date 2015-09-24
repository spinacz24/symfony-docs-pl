Walidacja poprzez ograniczenia
==============================

.. toctree::
   :maxdepth: 1
   :hidden:

   constraints/NotBlank
   constraints/Blank
   constraints/NotNull
   constraints/Null
   constraints/True
   constraints/False
   constraints/Type

   constraints/Email
   constraints/Length
   constraints/Url
   constraints/Regex
   constraints/Ip
   constraints/Uuid

   constraints/Range

   constraints/EqualTo
   constraints/NotEqualTo
   constraints/IdenticalTo
   constraints/NotIdenticalTo
   constraints/LessThan
   constraints/LessThanOrEqual
   constraints/GreaterThan
   constraints/GreaterThanOrEqual

   constraints/Date
   constraints/DateTime
   constraints/Time

   constraints/Choice
   constraints/Collection
   constraints/Count
   constraints/UniqueEntity
   constraints/Language
   constraints/Locale
   constraints/Country

   constraints/File
   constraints/Image

   constraints/CardScheme
   constraints/Currency
   constraints/Luhn
   constraints/Iban
   constraints/Isbn
   constraints/Issn

   constraints/Callback
   constraints/Expression
   constraints/All
   constraints/UserPassword
   constraints/Valid

Walidator został zaprojektowany do sprawdzania prawidłowości obiektów względem
okreśłonych *ograniczeń*. W codziennym życiu takim ograniczeniem może być zasada:
"To ciasto nie może być spalone". W Symfony ograniczenia są podobne: są twierdzeniami
(asercjami), że warunek jest prawdziwy.

Obsługiwane ograniczenia
------------------------

W Symfony natywnie dostępne są następujące ograniczenia:

.. include:: /reference/constraints/map.rst.inc
