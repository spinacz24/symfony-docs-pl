.. index::
   single: bezpieczeństwo; access_control
   single: konfiguracja; access_control

Jak działa access_control?
==========================

Przy każdym żądaniu Symfony sprawdza w pliku ``security.yml`` sekcję ``access_control``
w celu znalezienia takiego jednego wpisu, który pasuje do bieżącego żądania. Jak tylko
taki wpis zostanie znaleziony, wyszukiwanie jest zatrzymywane - tak więc do udzielenia
dostępu zostanie zastosowany **pierwszy** dopasowany wpis ``access_control``.

Sekcja ``access_control`` ma kilka opcji, które związane są z dwoma różnymi działaniami:

#. :ref:`dopasowanie przychodzacego żądania do wpisu kontroli dostępu <security-book-access-control-matching-options>`
#. :ref:`po dopasowaniu, wyegzekwowanie jakiegoś rodzaju ograniczenia <security-book-access-control-enforcement-options>`:

.. _security-book-access-control-matching-options:

1. Opcje dopasowujące
---------------------

Dla każdego wpisu ``access_control``, Symfony tworzy instancję klasy
:class:`Symfony\\Component\\HttpFoundation\\RequestMatcher`, która określa, czy
dana reguła kontroli dostępu powinna być zastosowana dla bieżącego żądania.
Przy dopasowaniu stosowane są następujące opcje ``acess_control``:

* ``path``
* ``ip`` lub ``ips``
* ``host``
* ``methods``

Dla przykładu zastosujmy następujące wpisy ``access_control``::

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            access_control:
                - { path: ^/admin, roles: ROLE_USER_IP, ip: 127.0.0.1 }
                - { path: ^/admin, roles: ROLE_USER_HOST, host: symfony\.com$ }
                - { path: ^/admin, roles: ROLE_USER_METHOD, methods: [POST, PUT] }
                - { path: ^/admin, roles: ROLE_USER }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <rule path="^/admin" role="ROLE_USER_IP" ip="127.0.0.1" />
                <rule path="^/admin" role="ROLE_USER_HOST" host="symfony\.com$" />
                <rule path="^/admin" role="ROLE_USER_METHOD" methods="POST, PUT" />
                <rule path="^/admin" role="ROLE_USER" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'access_control' => array(
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER_IP',
                    'ip' => '127.0.0.1',
                ),
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER_HOST',
                    'host' => 'symfony\.com$',
                ),
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER_METHOD',
                    'methods' => 'POST, PUT',
                ),
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER',
                ),
            ),
        ));

Symfony decyduje która reguła ``access_control`` zostanie użyta dla 
przychodzącego żądania w oparciu o adres URI, adres IP klienta, nadesłane nazwy
hosta i metody żądania. Trzeba pamiętać, że użyta zostaje pierwsza dopasowana
reguła i jeśli wartości ``ip``, ``host`` lub ``method`` nie są określone we wpisie,
to ``access_control`` będzie dopasowywał każdy ``ip``, ``host`` lub ``method``:

+-----------------+-----------+-------------+------------+----------------------------------+--------------------------------------------------------------+
| **URI**         | **IP**    | **HOST**    | **METHOD** | ``access_control``               | Dlaczego?                                                    |
+=================+===========+=============+============+==================================+==============================================================+
| ``/admin/user`` | 127.0.0.1 | example.com | GET        | reguła #1 (``ROLE_USER_IP``)     | Adres URI dopasowuje ``path`` a IP dopasowuje ``ip``.        |
+-----------------+-----------+-------------+------------+----------------------------------+--------------------------------------------------------------+
| ``/admin/user`` | 127.0.0.1 | symfony.com | GET        | reguła #1 (``ROLE_USER_IP``)     | ``path`` i ``ip`` nadal są dopasowywane. Dopasowywane jest   |
|                 |           |             |            |                                  | to również do reguły ``ROLE_USER_HOST``, ale użyta będzie    |
|                 |           |             |            |                                  | **tylko pierwsza** dopasowana  reguła ``access_control``.    |
+-----------------+-----------+-------------+------------+----------------------------------+--------------------------------------------------------------+
| ``/admin/user`` | 168.0.0.1 | symfony.com | GET        | reguła #2 (``ROLE_USER_HOST``)   | ``ip`` nie pasuje do pierwszej reguły, więc użyta będzie     |
|                 |           |             |            |                                  | druga reguła (jeśli bedzie pasować).                         |
+-----------------+-----------+-------------+------------+----------------------------------+--------------------------------------------------------------+
| ``/admin/user`` | 168.0.0.1 | symfony.com | POST       | reguła #2 (``ROLE_USER_HOST``)   | Stosowana jest dalej druga reguła. Wprawdzie pasuje to       |
|                 |           |             |            |                                  | również do trzeciej reguły (``ROLE_USER_METHOD``), ale użyta |
|                 |           |             |            |                                  | jest zawsze pierwsza dopasowana reguła ``access_control``.   |
+-----------------+-----------+-------------+------------+----------------------------------+--------------------------------------------------------------+
| ``/admin/user`` | 168.0.0.1 | example.com | POST       | reguła #3 (``ROLE_USER_METHOD``) | ``ip`` i ``host`` nie pasują do dwóch pierwszych reguł, ale  |
|                 |           |             |            |                                  | pasuja do trzeciej, ``ROLE_USER_METHOD``, która będzie użyta |
+-----------------+-----------+-------------+------------+----------------------------------+--------------------------------------------------------------+
| ``/admin/user`` | 168.0.0.1 | example.com | GET        | reguła #4 (``ROLE_USER``)        | ``ip``, ``host`` i ``method`` wykluczają dopasowanie trzech  |
|                 |           |             |            |                                  | pierwszych reguł. Lecz ponieważ adres URI dopasowuje wzorzec |
|                 |           |             |            |                                  | ``path`` reguły ``ROLE_USER``, to zostanie ona użyta.        |
+-----------------+-----------+-------------+------------+----------------------------------+--------------------------------------------------------------+
| ``/foo``        | 127.0.0.1 | symfony.com | POST       | brak pasujących wpisów           | Nie dopasowuje to żadnej reguły ``access_control``, ponieważ |
|                 |           |             |            |                                  | adres URI nie pasuje do jakiejkolwiek wartości``path``.      |
+-----------------+-----------+-------------+------------+----------------------------------+--------------------------------------------------------------+


.. _security-book-access-control-enforcement-options:

2. Egzekwowanie ograniczeń
--------------------------

Po tym jak Symfony określi, który wpis ``access_control`` zostanie użyty
(jeśli w ogóle), to następnie wymusza ograniczenie dostępu na podstawie opcji
``role``, ``allow_if`` i ``requires_channel``:

* ``role``: Jeśli użytkownik nie ma przydzielonej określonej roli (ról), to dostęp
  zostaje zabroniony (wewnętrznie zrzucany jest wyjątek
  :class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException`;
  
* ``requires_channel``: Jeśli kanał przychodzącego żądania (np. ``http``)
  nie zostaje dopasowany do tej wartości (np. ``https``), użytkownik zostanie
  przekierowany (np. przekierowany z ``http`` na ``https`` lub odwrotnie).

.. tip::

    W razie odmowy dostępu system będzie próbował uwierzytelnić użytkownika,
    jeśli nie jest on uwierzytelniony (np. przekierować użytkownika do strony
    logowania). Jeśli użytkownik jest już zalogowany, to zostanie wyświetlona
    strona błędu 403 "access denied". Przeczytaj artykuł
    :doc:`Jak dostosować strony błedów</cookbook/controller/error_pages>`.


.. index::
   single: bezpieczeństwo; zabezpieczenie prze IP

.. _book-security-securing-ip:

Zabezpieczanie przez IP
~~~~~~~~~~~~~~~~~~~~~~~

W pewnych sytuacjach moze zachodzić potrzeba sformułowania wpisu ``access_control``,
który dopasowuje żądania przychodzące tylko z jakiegość adresu IP lub zakresu
takich adresów. Można to wykorzystać, na przykład, do ograniczenia dostępu do
adresów pasujących do jakiegoś wzorca URL, z wyjatkiem żądań przychodzących
z zaufanego serwera wewnetrznego.

.. caution::

    Jak mozna przeczytać w wyjaśnieniu ponizeszego przykładu, opcja ``ips``
    nie ogranicza się do konkretnego adresu IP. Zastosowanie klucza ``ips``
    oznacza, że ten wpis ``access_control`` będzie tylko dopasowywał tylko ten
    adres IP i udzielał użytkownikom nadal dostępu z innych adresów IP wyszczególnionych
    dalej na liście ``access_control``.

Oto przykład, jak można skonfigurować przykładowy wzorzec URL ``/internal*``, tak
aby dostępne były tylko żądania z lokalnego serwera:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            access_control:
                #
                - { path: ^/internal, roles: IS_AUTHENTICATED_ANONYMOUSLY, ips: [127.0.0.1, ::1] }
                - { path: ^/internal, roles: ROLE_NO_ACCESS }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <rule path="^/internal"
                    role="IS_AUTHENTICATED_ANONYMOUSLY"
                    ips="127.0.0.1, ::1"
                />

                <rule path="^/internal" role="ROLE_NO_ACCESS" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'access_control' => array(
                array(
                    'path' => '^/internal',
                    'role' => 'IS_AUTHENTICATED_ANONYMOUSLY',
                    'ips' => '127.0.0.1, ::1'
                ),
                array(
                    'path' => '^/internal',
                    'role' => 'ROLE_NO_ACCESS'
                ),
            ),
        ));

Oto jak to działa dla żądania ze ścieżką ``/internal/something`` przychodzacego
z zewnętrznego adresu ``10.0.0.1``:

* Pierwsza reguła kontroli dostępu zostaje zignorowana, jako że ``path`` wprawdzie
  pasuje, ale nie zgadza się z jednym z wymienionych adresów IP;

* Druga reguła kontroli dostępu zostaje włączona (jedynym ograniczeniem jest ``path``,
  które pasuje) - jako że użytkownik nie może mieć roli ``ROLE_NO_ACCESS``, której
  nie określono, dostęp zostaje zabroniony (rola ``ROLE_NO_ACCESS`` może być czymś,
  co nie pasuje do żadnej roli, to po prostu tylko trik, zawsze uniemożliwiający
  dostęp).

Teraz, gdy to samo żądanie przyjdzie z serwera ``127.0.0.1`` lub ``::1`` (adres
pętli zwrotnej IPv6):

* Pierwsza reguła kontroli dostępu zostaje włączona, gdyż zarówno ``path``
  jak i ``ips`` zostają dopasowane – dostęp jest dozwolony jako że użytkownik zawsze
  ma rolę ``IS_AUTHENTICATED_ANONYMOUSLY``;

* Druga reguła kontroli dostępu nie jest sprawdzana, bo dopasowana została już
  pierwsza reguła.


.. index::
   single: bezpieczeństwo; zabezpieczenie przez wyrażenie

.. _book-security-allow-if:

Zabezpieczanie przez wyrażenie
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Po dopasowaniu wpisu ``access_control``, mozna odmówić dostępu stosując klucz
``roles`` lub używając bardziej złozonej logiki w postaci wyrażenia w kluczu
``allow_if``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            access_control:
                -
                    path: ^/_internal/secure
                    allow_if: "'127.0.0.1' == request.getClientIp() or has_role('ROLE_ADMIN')"

    .. code-block:: xml

            <access-control>
                <rule path="^/_internal/secure"
                    allow-if="'127.0.0.1' == request.getClientIp() or has_role('ROLE_ADMIN')" />
            </access-control>

    .. code-block:: php

            'access_control' => array(
                array(
                    'path' => '^/_internal/secure',
                    'allow_if' => '"127.0.0.1" == request.getClientIp() or has_role("ROLE_ADMIN")',
                ),
            ),

W tym przypadku, gdy użytkownik bedzie próbował uzyskać dostęp do zasobów ze
ścieżkami rozpoczynającymi się od ``/_internal/secure``, to będzie mu przyznawany
dostęp tylko wtedy, gdy żądanie wysłano z adresu IP ``127.0.0.1`` lub jeśli
użytkownik ma rolę ``ROLE_ADMIN``.

Wewnątrz wyrażenia ma się dostęp do wielu różnych zmiennych i funkcji dołączających
``request``, co w Symfony jest obiektem klasy
:class:`Symfony\\Component\\HttpFoundation\\Request`
(patrz :ref:`component-http-foundation-request`).

Pełny wykaz tych funkcji i zmiennych znajduje sie w 
:ref:`functions and variables <book-security-expression-variables>`.

.. index::
   single: bezpieczeństwo; wymuszanie kanału https

.. _book-security-securing-channel:

Wymuszanie kanału https
-----------------------

Można również wymmagać, aby użytkownik uzyskiwał dostęp URL poprzez SSL.
Wystarczy we wpisie ``access_control`` użyć argument ``requires_channel``.
Jeśli zostanie dopasowany ten wpis ``access_control`` a żądanie używa kanału ``http`,
to uzytkownik zostanie przekierowany do ``https``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            access_control:
                - { path: ^/cart/checkout, roles: IS_AUTHENTICATED_ANONYMOUSLY, requires_channel: https }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <rule path="^/cart/checkout"
                role="IS_AUTHENTICATED_ANONYMOUSLY"
                requires-channel="https"
            />
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'access_control' => array(
                array(
                    'path' => '^/cart/checkout',
                    'role' => 'IS_AUTHENTICATED_ANONYMOUSLY',
                    'requires_channel' => 'https',
                ),
            ),
        ));
