Po wywolaniu przez Symfony metody ``createToken()``, zostanie wywołana metoda
``supportsToken()`` (i jakiekolwiek inne detektory uwierzytelniania) w celu
uzyskania informacji, jak powinien być obsłużony ten token. Jest to sposób, aby
umożliwić uzycie w tej samej zaporze kilku mechanizmom uwierzytelniania (w ten
sposób można, na przykład, najpierw spróbować uwierzytelniania użytkownika poprzez
certyfikat lub klucz API i w razie niepowodzenia powrócić do logowania formularzoweg).

Najczęściej, wystarczy upewnić się, że ta metoda zwraca ``true`` dla tokenu
utworzonego przez metodę ``createToken()``. Twój kod logiki powinien przypuszczalnie
wyglądać dokladnie tak, jak w tym przykładzie.
