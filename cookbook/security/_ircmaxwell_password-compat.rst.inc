.. caution::

    Jeśli używa się PHP 5.4 lub wersji niższej, trzeba zainstalować poprzez
    Composer bibliotekę ``ircmaxell/password-compat`` w celu umożliwienia szyfrowania
    haseł algorytmem ``bcrypt``:

    .. code-block:: json

        {
            "require": {
                ...
                "ircmaxell/password-compat": "~1.0.3"
            }
        }
