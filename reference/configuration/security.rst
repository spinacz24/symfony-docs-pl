.. index::
   pair: system bezpieczeństwa; konfiguracja

Konfiguracja systemu bezpieczeństwa
===================================

System bezpieczeństwa jest jedną z najpotężniejszych części Symfony i może
w dużej mierze być kontrolowane poprzez konfigurację.

Pełna domyślna konfiguracja
---------------------------

Poniżej znajduje się pełna domyślna konfiguracja systemu bezpieczeństwa.
Każda część zostanie wyjaśniona w następnym rozdziale.

.. versionadded:: 2.5
    Wsparcie dla ograniczania zapory bezpieczeństwa do określonych metod HTTP
    zostało wprowadzone w Symfony 2.5.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            access_denied_url:    ~ # Example: /foo/error403

            # strategy can be: none, migrate, invalidate
            session_fixation_strategy:  migrate
            hide_user_not_found:  true
            always_authenticate_before_granting:  false
            erase_credentials:    true
            access_decision_manager:
                strategy:             affirmative
                allow_if_all_abstain:  false
                allow_if_equal_granted_denied:  true
            acl:

                # any name configured in doctrine.dbal section
                connection:           ~
                cache:
                    id:                   ~
                    prefix:               sf2_acl_
                provider:             ~
                tables:
                    class:                acl_classes
                    entry:                acl_entries
                    object_identity:      acl_object_identities
                    object_identity_ancestors:  acl_object_identity_ancestors
                    security_identity:    acl_security_identities
                voter:
                    allow_if_object_identity_unavailable:  true

            encoders:
                # Examples:
                Acme\DemoBundle\Entity\User1: sha512
                Acme\DemoBundle\Entity\User2:
                    algorithm:           sha512
                    encode_as_base64:    true
                    iterations:          5000

                # PBKDF2 encoder
                # see the note about PBKDF2 below for details on security and speed
                Acme\Your\Class\Name:
                    algorithm:            pbkdf2
                    hash_algorithm:       sha512
                    encode_as_base64:     true
                    iterations:           1000
                    key_length:           40

                # Example options/values for what a custom encoder might look like
                Acme\DemoBundle\Entity\User3:
                    id:                   my.encoder.id

                # BCrypt encoder
                # see the note about bcrypt below for details on specific dependencies
                Acme\DemoBundle\Entity\User4:
                    algorithm:            bcrypt
                    cost:                 13

                # Plaintext encoder
                # it does not do any encoding
                Acme\DemoBundle\Entity\User5:
                    algorithm:            plaintext
                    ignore_case:          false

            providers:            # Required
                # Examples:
                my_in_memory_provider:
                    memory:
                        users:
                            foo:
                                password:           foo
                                roles:              ROLE_USER
                            bar:
                                password:           bar
                                roles:              [ROLE_USER, ROLE_ADMIN]

                my_entity_provider:
                    entity:
                        class:              SecurityBundle:User
                        property:           username
                        # name of a non-default entity manager
                        manager_name:       ~

                # Example custom provider
                my_some_custom_provider:
                    id:                   ~

                # Chain some providers
                my_chain_provider:
                    chain:
                        providers:          [ my_in_memory_provider, my_entity_provider ]

            firewalls:            # Required
                # Examples:
                somename:
                    pattern: .*
                    # restrict the firewall to a specific host
                    host: admin\.example\.com
                     # restrict the firewall to specific http methods
                    methods: [GET, POST]
                    request_matcher: some.service.id
                    access_denied_url: /foo/error403
                    access_denied_handler: some.service.id
                    entry_point: some.service.id
                    provider: some_key_from_above
                    # manages where each firewall stores session information
                    # See "Firewall Context" below for more details
                    context: context_key
                    stateless: false
                    x509:
                        provider: some_key_from_above
                    remote_user:
                        provider: some_key_from_above
                    http_basic:
                        provider: some_key_from_above
                    http_digest:
                        provider: some_key_from_above
                    form_login:
                        # submit the login form here
                        check_path: /login_check

                        # the user is redirected here when they need to log in
                        login_path: /login

                        # if true, forward the user to the login form instead of redirecting
                        use_forward: false

                        # login success redirecting options (read further below)
                        always_use_default_target_path: false
                        default_target_path:            /
                        target_path_parameter:          _target_path
                        use_referer:                    false

                        # login failure redirecting options (read further below)
                        failure_path:    /foo
                        failure_forward: false
                        failure_path_parameter: _failure_path
                        failure_handler: some.service.id
                        success_handler: some.service.id

                        # field names for the username and password fields
                        username_parameter: _username
                        password_parameter: _password

                        # csrf token options
                        csrf_parameter: _csrf_token
                        intention:      authenticate
                        csrf_provider:  my.csrf_provider.id

                        # by default, the login form *must* be a POST, not a GET
                        post_only:      true
                        remember_me:    false

                        # by default, a session must exist before submitting an authentication request
                        # if false, then Request::hasPreviousSession is not called during authentication
                        # new in Symfony 2.3
                        require_previous_session: true

                    remember_me:
                        token_provider: name
                        key: someS3cretKey
                        name: NameOfTheCookie
                        lifetime: 3600 # in seconds
                        path: /foo
                        domain: somedomain.foo
                        secure: false
                        httponly: true
                        always_remember_me: false
                        remember_me_parameter: _remember_me
                    logout:
                        path:   /logout
                        target: /
                        invalidate_session: false
                        delete_cookies:
                            a: { path: null, domain: null }
                            b: { path: null, domain: null }
                        handlers: [some.service.id, another.service.id]
                        success_handler: some.service.id
                    anonymous: ~

                # Default values and options for any firewall
                some_firewall_listener:
                    pattern:              ~
                    security:             true
                    request_matcher:      ~
                    access_denied_url:    ~
                    access_denied_handler:  ~
                    entry_point:          ~
                    provider:             ~
                    stateless:            false
                    context:              ~
                    logout:
                        csrf_parameter:       _csrf_token
                        csrf_provider:        ~
                        intention:            logout
                        path:                 /logout
                        target:               /
                        success_handler:      ~
                        invalidate_session:   true
                        delete_cookies:

                            # Prototype
                            name:
                                path:                 ~
                                domain:               ~
                        handlers:             []
                    anonymous:
                        key:                  4f954a0667e01
                    switch_user:
                        provider:             ~
                        parameter:            _switch_user
                        role:                 ROLE_ALLOWED_TO_SWITCH

            access_control:
                requires_channel:     ~

                # use the urldecoded format
                path:                 ~ # Example: ^/path to resource/
                host:                 ~
                ips:                  []
                methods:              []
                roles:                []
            role_hierarchy:
                ROLE_ADMIN:      [ROLE_ORGANIZER, ROLE_USER]
                ROLE_SUPERADMIN: [ROLE_ADMIN]


.. index::
   single: bezpieczeństwo; konfiguracja logowania formularzowego
   single: logowanie formularzowe; konfiguracja

.. _reference-security-firewall-form-login:

Konfiguracja logowania formularzowego
-------------------------------------

Gdy używa się detektora uwierzytelniania ``form_login`` pod  :term:`zaporą <zapora>`,
istnieje kilka opcji do skonfigurowania działania "logowania formularzowego".

Więcej szczegółów można znaleźć w  artykule „:doc:`/cookbook/security/form_login`”.


Formularz oraz proces logowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

login_path
..........

**typ**: ``string``, **domyślnie**: ``/login``

Jest to trasa lub ścieżka, do której zastanie przekierowany użytkownik (jeśli
opcja ``use_forward`` nie jest ustawiona na ``true``) gdy będzie próbował uzyskać
dostęp do zasobu chronionego, ale nie zostanie w pełni uwierzytelniony.
    
Ścieżka ta musi być dostępna dla zwykłego, nie uwierzytelnionego użytkownika,
inaczej można zapetlić przekierowanie. Szczegóły dostępne są w rozdziale
":ref:`Jak unikać typowych pułapek<book-security-common-pitfalls>`".

check_path
..........

**typ**: ``string``, **domyślnie**: ``/login_check`

Jest to trasa lub ścieżka, którą powinien zgłosić formularz logowania.
Zapora przechwytuje wszystkie żądania (domyślnie tylko żądania ``POST``)
dla tej ścieżki URL i przetworza zgłoszone dane logowania.
      
Trzeba się upewnić, że ta ścieżka podlega zaporze głównej (tj. nie należy
tworzyć oddzielnej zapory tylko dla ścieżki z ``check_path``).

use_forward
...........

**typ**: ``Boolean``, **domyślnie**: ``false``

Jeśli chce się aby użytkownik został przekazany do formularza
logowania zamiast przekierowany, to trzeba ustawić tą opcję na ``true``.

username_parameter
..................

**typ**: ``string``, **domyślnie**: ``_username``

Jest to nazwa pola która powinna być użyta dla pola reprezentującego
nazwę użytkownika w formularzu logowania. Gdy formularz logowania zostanie
zgłoszony z adresem ``check_path``, to system bezpieczeństwa będzie szukał
parametru o tej nazwie w tablicy POST.

password_parameter
..................

**typ**: ``string``, **domyślnie**: ``_password``

Jest to nazwa pola która powinno się użyć dla pola reprezentującego
hasło użytkownika w formularzu logowania. Gdy formularz logowania
zostanie zgłoszony na ``check_path``, system bezpieczeństwa będzie szukał parametru
o tej nazwie w tablicy POST.

post_only
.........

**typ**: ``Boolean``, domyślnie: ``true``

Domyślnie, zgłasza się formularz logowania ze ścieżką URL ``check_path``,
jako żądanie POST. Jeśli ustawi się tą opcję na ``false``, to zamiast tego,
można będzie zgłosić formularz jako żądanie GET.

Przekierowanie po zalogowaniu
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* ``always_use_default_target_path`` (typ: ``Boolean``, domyślnie: ``false``)
* ``default_target_path`` (typ: ``string``, domyślnie: ``/``)
* ``target_path_parameter`` (typ: ``string``, domyślnie: ``_target_path``)
* ``use_referer`` (typ: ``Boolean``, domyślnie: ``false``)


.. index::
   single: bezpieczeństwo; koder PBKDF2
   single: koder PBKDF2; konfiguracja

.. _reference-security-pbkdf2:

Stosowanie kodera PBKDF2: bezpieczeństwo i szybkość
---------------------------------------------------

Koder `PBKDF2`_ zapewnia wysoki poziom bezpieczeństwa kryptograficznego, spełniając
kryteria zalecane przez National Institute of Standards and Technology (NIST).

Przykład kodera ``pbkdf2`` można zobaczyć w bloku YAML na tej stronie.

Lecz stosowanie PBKDF2 ma również swoje minusy: używając go (z dużą liczbą
iteracji) spowalnia się przetwarzanie. Dlatego PBKDF2 należy używać ostrożnie
i świadomie.

Dobra konfiguracja wymaga użycia około 1000 iteracji i sha512 dla algorytmu mieszania.


.. index::
   single: bezpieczeństwo; koder BCrypt
   single: koder BCrypt; konfiguracja

.. _reference-security-pbkdf2:

.. _reference-security-bcrypt:

Stosowanie kodera BCrypt
------------------------

.. caution::

    Korzystanie z tego kodera wymaga albo stosowania PHP w wersji co najmniej 5.5
    albo zainstalowania poprzez Composer biblioteki `ircmaxell/password-compat`_.

.. configuration-block::

    .. code-block:: yaml
       :linenos:
       
        # app/config/security.yml
        security:
            # ...

            encoders:
                Symfony\Component\Security\Core\User\User:
                    algorithm: bcrypt
                    cost:      15

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <!-- ... -->
            <encoder
                class="Symfony\Component\Security\Core\User\User"
                algorithm="bcrypt"
                cost="15"
            />
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => array(
                    'algorithm' => 'bcrypt',
                    'cost'      => 15,
                ),
            ),
        ));

Opcja ``cost`` może przybierać wartość z zakresu ``4-31`` i określa ile razy hasło
będzie kodowane. Każde zwiększenie wartości ``cost`` powoduje *podwojenie* czasu
kodowania hasła.

Jeśli nie określi się wartości opcji ``cost``,  to domyślnie użyta będzie liczba ``13``.

.. note::

    W każdej chwili można zmienić wartość kosztu, nawet jeśli się ma jakieś hasła
    zakodowane z użyciem innej wartości kosztu. Nowe hasła będą kodowane z użyciem
    nowej wartości kosztu, a te istniejące będą odkodowywane z wartością kosztu
    użytego do ich zakodowania.

Sól jest generowana automatycznie dla każdego nowego hasła i nie musi być utrwalana.
Ponieważ zakodowane hasło zawiera sól użytą do jego zakodowania, to wystarczy
przechowywanie samego zakodowanego hasła.

.. note::

    Wszystkie zakodowane hasła mają długość ``60`` znaków, więc należy zabezpieczyć
    dla nich dostateczną ilość miejsca.

.. index::
   single: bezpieczeństwo; zapory
   single: zapora; konfiguracja

.. _reference-security-firewall-context:

Kontekst zapory
---------------

Większość aplikacji potrzebuje tylko jedną :ref:`zaporę <book-security-firewalls>`.
Lecz jeśli aplikacja ma korzystać z wielu zapór, trzeba mieć na uwadze, że jeśli
jest się uwierzytelnionym w jednej zaporze to nie jest się automatycznie uwierzytelnionym
w pozostałych. Innymi słowami, systemy te nie współdzielą wspólnego "kontekstu" -
każda zapora działa jak odrębny system bezpieczeństwa.

Jednak każda zapora ma opcjonalny klucz ``context`` (którego domyślną wartością jest
nazwa zapory) wykorzystywaną podczas zapisu i pobierania danych bezpieczeństwa
oraz dla sesji. Jeśli klucz ten będzie ustawiony na tą samą wartość co pozostałe
zapory, to "kontekst" będzie w rzeczywistości współdzielony:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                somename:
                    # ...
                    context: my_context
                othername:
                    # ...
                    context: my_context

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <security:config>
            <firewall name="somename" context="my_context">
                <! ... ->
            </firewall>
            <firewall name="othername" context="my_context">
                <! ... ->
            </firewall>
        </security:config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'somename' => array(
                    // ...
                    'context' => 'my_context'
                ),
                'othername' => array(
                    // ...
                    'context' => 'my_context'
                ),
            ),
        ));

.. index::
   HTTP-Digest
   single: uwierzytelnianie; HTTP-Digest
   single: bezpieczeństwo; uwierzytelnianie HTTP-Gigest

Uwierzytelnianie HTTP-Digest
----------------------------

Dla stosowania uwierzytelniania `HTTP-Digest <http://www.faqs.org/rfcs/rfc2617.html>`_,
potrzeba dostarczyć dziedzinę (*ang. realm*)  i klucz:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                somename:
                    http_digest:
                        key: "a_random_string"
                        realm: "secure-api"

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <security:config>
            <firewall name="somename">
                <http-digest key="a_random_string" realm="secure-api" />
            </firewall>
        </security:config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'somename' => array(
                    'http_digest' => array(
                        'key'   => 'a_random_string',
                        'realm' => 'secure-api',
                    ),
                ),
            ),
        ));

.. _`PBKDF2`: http://en.wikipedia.org/wiki/PBKDF2
.. _`ircmaxell/password-compat`: https://packagist.org/packages/ircmaxell/password-compat