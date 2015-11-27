.. highlight:: php
   :linenothreshold: 2

.. index::
   ochrona dostępu, system bezpieczeństwa, bezpieczeństwo

System bezpieczeństwa
=====================

System bezpieczeństwa Symfony jest niezwykle silny, ale może być też błędnie ustawiony.
W tym rozdziale dowiesz się, krok po kroku, jak skonfigurować bezpieczeństwo swojej
aplikacji, począwszy od skonfigurowania zapory aż po informacje o tym, jak ustawiać
prawa dostępu dla użytkowników i pobierać obiekt User. W zależności od potrzeb,
początkowa konfiguracja może może być trudna, ale gdy się ją ustawi system bezpieczeństwa
stanie się elastyczny, jak i (miejmy nadzieję) ciekawy.

Ponieważ jest to temat bardzo szeroki, rozdział ten został podzielony na kilka
dużych podrozdziałów:

#. Początkowa konfiguracja ``security.yml`` (*uwierzytelnianie*);

#. Odmowa dostępu do aplikacji (*autoryzacja*);

#. Pobieranie obiektu bieżącego użytkownika.

Znajduje się tu też kilka małych podrozdziałów o charakterze technicznym,
takich jak :ref:`wylogowywanie <book-security-logging-out>` czy
:ref:`kodowanie haseł użytkowników <security-encoding-password>`.

.. index::
   zapory, uwierzylenianie
   single: bezpieczeństwo; uwierzytelnianie
   single: bezpieczeństwo; zapory
   single: uwierzylenianie; zapory

.. _book-security-firewalls:

1) Początkowa konfiguracja ``security.yml`` (uwierzytelnianie)
--------------------------------------------------------------

System bezpieczeństwa jest skonfigurowany w pliku ``app/config/security.yml``.
Domyślna konfiguracja wygląda tak:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            providers:
                in_memory:
                    memory: ~

            firewalls:
                dev:
                    pattern: ^/(_(profiler|wdt)|css|images|js)/
                    security: false

                main:
                    anonymous: ~

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <provider name="in_memory">
                    <memory />
                </provider>

                <firewall name="dev"
                    pattern="^/(_(profiler|wdt)|css|images|js)/"
                    security="false" />

                <firewall name="main">
                    <anonymous />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'providers' => array(
                'in_memory' => array(
                    'memory' => null,
                ),
            ),
            'firewalls' => array(
                'dev' => array(
                    'pattern'    => '^/(_(profiler|wdt)|css|images|js)/',
                    'security'   => false,
                ),
                'main' => array(
                    'anonymous'  => null,
                ),
            ),
        ));

Klucz ``firewalls`` jest *sercem* konfiguracji bezpieczeństwa i reprezentuje
element mechanizmu bezpieczeństwa Symfony nazywanego :term:`zaporą <zapora>` (*ang.firewall*).
O zaporze można myśleć jak o swoim systemie bezpieczeństwa i w tym sensie występuje
zazwyczaj tylko jedna główna zapora.
W prezentowanej konfiguracji, zapora ``dev`` zapewnia nie blokowanie narzędzi
programistycznych Symfony, które umieszczone są pod adresami URL takimi jak
``/_profiler`` i ``/_wdt``.

.. tip::

    Można również dopasować żądanie wobec innych szczegółów żądania (np. host).
    Wiecej informacji i przykłady znajdziesz w :doc:`/cookbook/security/firewall_restriction`.

Wszystkie inne adresy URL są obsługiwane przez zaporę ``main``. Brak klucza
``pattern`` oznacza, że dopasowywane są tu *wszystkie* adresy URL. Nie oznacza to,
że uwierzytelniania wymaga każdy adres URL - dba o to klucz ``anonymous``.
W rzeczywistości, jeśli przejdzie się teraz do strony początkowej, to będzie się
miało do niej dostęp będąc "uwierzytelnionym" jako ``anon``. Nie daj się zwieść
etykiecie *Yes* stojącej przy pozycji *Authenticated* w opcjach uwierzytelniania
paska narzedziowego debugowania - jesteś teraz anonimowym użytkownikiem:

.. image:: /images/book/security_anonymous_wdt.png
   :align: center

Później dowiesz się jak zablokować dostęp do określonych adresów URL lub akcji.

.. tip::

    System bezpieczeństwa w Symfony jest wysoce konfigurowalny, co omówione zostało
    w dokumencie :doc:`Informacje o konfiguracji bezpieczeństwa </reference/configuration/security>`,
    gdzie pokazano wszystkie opcje z dodatkowym wyjaśnieniem.

.. _book-security-firewalls_basic_config:

A) Skonfigurowanie sposobu uwierzytelniania użytkowników
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Węzeł ``firewall`` odpowiedzialny jest za skonfigurowanie tego *jak* użytkownicy
będą uwierzytelniani. Czy będzie używane logowanie formularzowe, czy może podstawowe
uwierzytelnianie HTTP? Czy będzie używane API tokenu? Czy też wszystko
razem?

Rozpocznijmy od podstawowego uwierzytelniania HTTP (monit starej szkoły) i popracujmy
z tym mechanizmem.
W celu jego aktywowania, dodajmy do węzła ``firewall`` klucz ``http_basic``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                # ...
                main:
                    anonymous: ~
                    http_basic: ~

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

                <firewall name="main">
                    <anonymous />
                    <http-basic />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'firewalls' => array(
                // ...
                'main' => array(
                    'anonymous'  => null,
                    'http_basic' => null,
                ),
            ),
        ));

Proste! Do wypróbowania tego trzeba ustawić wymóg logowania się przez użytkownika
na stronie. Żeby było interesujaco utworzymy nową stronę ``/admin``. Jeśli używasz
adnotacji stwórz coś takiego::

    // src/AppBundle/Controller/DefaultController.php
    // ...

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Component\HttpFoundation\Response;

    class DefaultController extends Controller
    {
        /**
         * @Route("/admin")
         */
        public function adminAction()
        {
            return new Response('<html><body>Admin page!</body></html>');
        }
    }

Następnie trzeba w pliku konfiguracyjnym ``security.yml`` dodać wpis ``access_control``,
który wymaga uwierzytelnienia użytkownika dla tego adresu URL:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            firewalls:
                # ...
                main:
                    # ...

            access_control:
                # require ROLE_ADMIN for /admin*
                - { path: ^/admin, roles: ROLE_ADMIN }

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

                <firewall name="main">
                    <!-- ... -->
                </firewall>

                <!-- require ROLE_ADMIN for /admin* -->
                <rule path="^/admin" role="ROLE_ADMIN" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'firewalls' => array(
                // ...
                'main' => array(
                    // ...
                ),
            ),
           'access_control' => array(
               // require ROLE_ADMIN for /admin*
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
        ));

.. note::

    O tym co oznacza rola ``ROLE_ADMIN`` i jak działa odmowa dostęþu dowiesz
    się później w rozdziale :ref:`security-authorization`.

Teraz, jeśli się spróbuje wywołać strone ``/admin``, zobaczy się monit podstawowego
uwierzytelnianina HTTP:

.. image:: /images/book/security_http_basic_popup.png
   :align: center

Lecz kto może się zalogować? Gdzie przetrzymywana jest informacja o użytkownikach?

.. _book-security-form-login:

.. tip::

    Chcesz wykorzystać tradycyjny formularz logowania? Proste! Przeczytaj artykuł
    :doc:`/cookbook/security/form_login_setup`.
    Czy obsługiwane są inne metody uwierzytelniania? Przeczytaj
    :doc:`Informator konfiguracji </reference/configuration/security>`
    lub :doc:`zbuduj własną metodę </cookbook/security/custom_authentication_provider>`.

.. tip::

    Jeśli aplikacja ma logować użytkownikow za pośrednictwem zewnętrznych serwisów,
    takich jak Facebook lub Twitter, zapoznaj się ze społecznościowym pakietem
    `HWIOAuthBundle`_.

.. _security-user-providers:
.. _where-do-users-come-from-user-providers:

B) Konfigurowanie sposobu ładowania informacji o użytkownikach
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W celu dokonania uwierzytelniania, Symfony musi załadować skądś informacje o
użytkowniku (jego nazwę i hasło). Miejsce to nazywane jest "dostawcą użytkowników"
i można go ustawić w konfiguracji systemu bezpieczeństwa. Symfony ma wbudowany
mechanizm :doc:`ładowania użytkowników z bazy danych </cookbook/security/entity_provider>`,
lub można :doc:`utworzyć własnego dostawcę użytkowników </cookbook/security/custom_provider>`.

Najprostszym (ale bardzo ograniczonym) sposobem jest skonfigurowanie Symfony do
sztywnego ładowania informacji o użytkownikach bezpośrednio z pliku konfiguracyjnego
(``security.yml``). Ten sposób nosi nazwę "dostawcy z pamięci" (*ang. in-memory*):

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            providers:
                in_memory:
                    memory:
                        users:
                            ryan:
                                password: ryanpass
                                roles: 'ROLE_USER'
                            admin:
                                password: kitten
                                roles: 'ROLE_ADMIN'
            # ...

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <provider name="in_memory">
                    <memory>
                        <user name="ryan" password="ryanpass" roles="ROLE_USER" />
                        <user name="admin" password="kitten" roles="ROLE_ADMIN" />
                    </memory>
                </provider>
                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'providers' => array(
                'in_memory' => array(
                    'memory' => array(
                        'users' => array(
                            'ryan' => array(
                                'password' => 'ryanpass',
                                'roles' => 'ROLE_USER',
                            ),
                            'admin' => array(
                                'password' => 'kitten',
                                'roles' => 'ROLE_ADMIN',
                            ),
                        ),
                    ),
                ),
            ),
            // ...
        ));

.. note::
   Użytkownikowi możemy przypisać kilka ról. Można to wykonać ustalając wartość
   klucza ``roles`` w postaci listy wartości, np.::
         
         'roles' => ['ROLE_USER','ROLE_ADMIN']
   
   ale lepszym sposobem jest wykoszzytstanie ról hierarchicznych, co zostanie omówione dalej. 



Podobnie jak w przypadku ``firewalls``, można mieć wiele ustawień ``providers``,
ale w większości przypadków jest to zbędne. Jeśli skonfiguruje się wielu dostawców,
to można skonfigurować, że poszczególni dostawcy używają zapory pod własnym kluczem
``provider`` (np. ``provider: in_memory``).

.. seealso::

    Przeczytaj :doc:`/cookbook/security/multiple_user_providers` w celu zapoznania
    się ze szczegółami konfigurowania wielu dostawców.

Wypróbuj teraz zalogować użytkownika ``admin`` stosując hasło ``kitten``. Zobaczysz
komunikat o błędzie::

    No encoder has been configured for account "Symfony\\Component\\Security\\Core\\User\\User"

Musimy więc dodać do konfiguracji klucz ``encoders``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            encoders:
                Symfony\Component\Security\Core\User\User: plaintext
            # ...

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

                <encoder class="Symfony\Component\Security\Core\User\User"
                    algorithm="plaintext" />
                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => 'plaintext',
            ),
            // ...
        ));

Dostawca użytkowników ładuje informacje o użytkowniku i wstawia je do obiektu
``User``. Jeśli :doc:`ładuje się użytkowników z bazy danych </cookbook/security/entity_provider>`
lub :doc:`z jakiegoś własnego źródła </cookbook/security/custom_provider>`,
trzeba użyć własnej klasy User. Natomiast w przypadku korzystania z dostawcy
"w pamięci" ma się gotowy obiekt ``Symfony\Component\Security\Core\User\User``.

Ponadto trzeba powiadomić Symfony jaki algorytm szyfrowania haseł się stosuje.
W naszym przypadku zastosowaliśmy zwykły tekst, ale można to bez problemu zmienić
na koder taki ``bcrypt`` czy ``pbkdf2`` - patrz :ref:`reference-security-bcrypt`
i :ref:`reference-security-pbkdf2`.

Jeśli teraz odświeżysz przeglądarkę, okaże się że użytkownik ``admin`` jest już
zalogowany! Wskazuje na to nie tylko możliwość dostępu do stron administracyjnych,
ale również informacja na pasku narzędziowym debugowania:

.. image:: /images/book/symfony_loggedin_wdt.png
   :align: center

Ponieważ dostęp do stron administracyjnych wymaga roli ``ROLE_ADMIN``, to po zalogowaniu
się jako ``ryan``, otrzymasz odmowę dostępu. Zagadnienie to zostanie omówione
później (:ref:`security-authorization-access-control`).

.. _book-security-user-entity:

Ładowanie użytkowników z bazy danych
....................................

Ładowanie informacji o użytkownikach z wykorzystaniem Doctrine ORM jest proste.
Proszę przeczytać artykuł :doc:`/cookbook/security/entity_provider` szczegółowo
omawiający to zagadnienie.

.. _book-security-encoding-user-password:
.. _c-encoding-the-users-password:
.. _encoding-the-user-s-password:

C) Kodowanie haseł użytkowników
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Niezależnie od tego, czy użytkownicy są przechowywani w pliku ``security.yml``,
bazie danych, czy też gdzie indziej, będziemy musieli szyfrować ich hasła.
Najlepszym algorytmem jest ``bcrypt``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            encoders:
                Symfony\Component\Security\Core\User\User:
                    algorithm: bcrypt
                    cost: 12

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

                <encoder class="Symfony\Component\Security\Core\User\User"
                    algorithm="bcrypt"
                    cost="12" />

                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => array(
                    'algorithm' => 'bcrypt',
                    'cost' => 12,
                )
            ),
            // ...
        ));

.. include:: /cookbook/security/_ircmaxwell_password-compat.rst.inc

Oczywiście, trzeba teraz zakodować istniejące hasła tym algorytmem.
Dla sztywno kodowanych użytkowników można wykorzytać `narzędzie online`_,
które daje coś takiego:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            providers:
                in_memory:
                    memory:
                        users:
                            ryan:
                                password: $2a$12$LCY0MefVIEc3TYPHV9SNnuzOfyr2p/AXIGoQJEDs4am4JwhNz/jli
                                roles: 'ROLE_USER'
                            admin:
                                password: $2a$12$cyTWeE9kpq1PjqKFiWUZFuCRPwVyAZwm4XzMZ1qPUFl7/flCM3V0G
                                roles: 'ROLE_ADMIN'

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

                <provider name="in_memory">
                    <memory>
                        <user name="ryan" password="$2a$12$LCY0MefVIEc3TYPHV9SNnuzOfyr2p/AXIGoQJEDs4am4JwhNz/jli" roles="ROLE_USER" />
                        <user name="admin" password="$2a$12$cyTWeE9kpq1PjqKFiWUZFuCRPwVyAZwm4XzMZ1qPUFl7/flCM3V0G" roles="ROLE_ADMIN" />
                    </memory>
                </provider>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'providers' => array(
                'in_memory' => array(
                    'memory' => array(
                        'users' => array(
                            'ryan' => array(
                                'password' => '$2a$12$LCY0MefVIEc3TYPHV9SNnuzOfyr2p/AXIGoQJEDs4am4JwhNz/jli',
                                'roles' => 'ROLE_USER',
                            ),
                            'admin' => array(
                                'password' => '$2a$12$cyTWeE9kpq1PjqKFiWUZFuCRPwVyAZwm4XzMZ1qPUFl7/flCM3V0G',
                                'roles' => 'ROLE_ADMIN',
                            ),
                        ),
                    ),
                ),
            ),
            // ...
        ));

Wszystko teraz będzie działać dokładnie, jak poprzednio. Lecz co zrobić, gdy ma
się dynamicznych użytkowników (np. z bazy danych), jak można programowo zakodować
hasło przed wstawieniem go do bazy danych? Nie martw się, omówimy to ze szczegółami
w rozdziale :ref:`security-encoding-password`.

.. tip::

    Obsługiwane algorytmy dla tej metody zależą od wersji PHP, lecz zawarte są 
    algorytmy zwracane przez funkcję PHP function :phpfunction:`hash_algos`
    jak też wiele innych (np. bcrypt). Proszę zapoznać się z opisem klucza ``encoders``
    w dokumencie :doc:`Security Reference Section </reference/configuration/security>`.

    Jest też możliwe użycie innych algorytmów mieszających na bazie *user-by-user*.
    Zobacz :doc:`/cookbook/security/named_encoders` dla poznania szczegółów.

D) Konfiguracja wykonana!
~~~~~~~~~~~~~~~~~~~~~~~~~

W ten sposób mamy działajacy system uwierzytelniania, który wykorzystuje podstawowe
uwierzytelnianie HTTP i ładuje dane użytkowników z pliku konfiguracyjnego ``security.yml``.

Kolejne kroki mogą obejmować:

* Skonfigurowanie innego sposobu logowania użytkownika, takiego jak
  :ref:`formularz logowania <book-security-form-login>` lub
  :doc:`coś zupełnie własnego </cookbook/security/custom_authentication_provider>`;

* Załadowanie użytkownika z innego źródła, jak na przykład z
  :doc:`bazy danych </cookbook/security/entity_provider>`
  lub :doc:`jakiegoś innego źródła </cookbook/security/custom_provider>`;

* Poznanie, jak można odmawiać dostępu, ładować obiekt User i poradzić sobie z
  rolami, co jest opisane w rozdziale :ref:`Autoryzacja <security-authorization>`.

.. index::
   autoryzacja
   single: system bezpieczeństwa; autoryzacja

.. _`security-authorization`:

2) Odmowa dostępu, role i inna autoryzacja
------------------------------------------

Użytkownicy mogą teraz logować się do aplikacji używając ``http_basic``lub innej
metody.
Przyszedł więc czas na poznanie, jak odmawiać dostępu i pracować z obiektem
User.
Nazywa się to **autoryzacją**, a jej zadaniem jest decydowanie, czy użytkownik
może uzyskać dostęp do pewnych zasobów (ścieżki URL, obiektu modelu, wywołania
metody itd.).

Proces autoryzacji ma dwa aspekty:

#. Użytkownik uzyskuje określony zestaw ról podczas logowania (np. ``ROLE_ADMIN``).
#. Można dodać kod, tak aby zasób (np. URL, kontroler) wymagał od użytkownika
   określonego "atrybutu" (najczęściej roli, takiej jak ``ROLE_ADMIN``), aby być
   dostępny.

.. tip::

    Oprócz ról (np. ``ROLE_ADMIN``), do ochrony zasobów można wykorzystywać
    inne atrybuty (ciągi znakowe) (np. ``EDIT``) oraz używać wyborców lub systemu
    ACL Symfony. Moze to być przydatne, jeśli potrzeba sprawdzić, czy użytkownik A
    może edytować ("EDIT") jakiś obiekt B (np. Product z id 5).
    Zobacz :ref:`security-secure-objects`.

.. index::
   role
   single: autoryzacja; role

.. _book-security-roles:

Role
~~~~

Podczas logowania użytkownika, otrzymuje on zestaw ról (np. ``ROLE_ADMIN``).
W powyższym przykładzie, są one sztywno przypisane użytkownikowi w pliku ``security.yml``.
Jeśli ładuje się użytkowników z bazy danych, role są prawdopodobnie przechowywane
w kolumnie tabeli.

.. caution::

    Wszystkie role przypisane do użytkownika **muszą** być poprzedzone przedrostkiem
    ``ROLE_``.
    W przeciwnym razie nie będą one obsługiwane przez system bezpieczeństwa
    Symfony w normalny sposób (chyba że wykonało się jakiś zaawansowany kod,
    przypisanie roli takiej jak ``FOO`` do użytkownika i sprawdzaanie ``FOO``, tak
    jak opisano to :ref:`poniżej <security-role-authorization>` nie zadziała).

Role są proste i w zasadzie są to tylko ciągi znakowe, które są nadawane
i wykorzystywane w razie potrzeby.
Na przykład, jeśli chcesz ograniczyć dostęp do secji administracyjnej bloga witryny,
możesz ją zabezpieczyć stosując rolę ``ROLE_BLOG_ADMIN``. Rola ta nie musi być
zdefiniowana w jakimś konkretnym miejscu - po prostu wystarczy ją używać.

.. tip::

    Koniecznym jest, aby każdy użytkownik posiadał co najmniej jedną rolę, gdyż
    w przeciwnym razie, będzie traktowany jako nie uwierzytelniany. Powszechną
    konwencją jest nadawania każdemu użytkownikowi roli ``ROLE_USER``.

Można również określić :ref:`hierarchię ról <security-role-hierarchy>`, w której
role podrzędne będą automatycznie posiadać role nadrzędne.

.. _security-role-authorization:

Dodawanie kodu blokującego dostęp
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Są dwa sposoby zablokowania dostępu do jakiegoś elementu:

#. :ref:`kontrola dotępu w security.yml <security-authorization-access-control>`
   umożliwia zabezpieczyć wzorce  URL (np. ``/admin/*``). Jest to łatwe, ale mniej
   elastyczne;

#. :ref:`w swoim kodzie poprzez usługę security.authorization_checker <book-security-securing-controller>`.

.. _security-authorization-access-control:

Zabezpieczenie wzorców URL (access_control)
...........................................

Najprostszym sposobem zabezpieczenia części swojej aplikacji jest zabezpieczenie
całego wzorca URL. Widzieliśmy już to wcześniej w postaci użycia klucza w sekcji
``access_control``  dopasowującym wyrażenie regularne  ``^/admin`` i wymagającym
roli ``ROLE_ADMIN`` dla takiej ścieżki:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                # ...
                default:
                    # ...

            access_control:
                # require ROLE_ADMIN for /admin*
                - { path: ^/admin, roles: ROLE_ADMIN }

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

                <firewall name="default">
                    <!-- ... -->
                </firewall>

                <!-- require ROLE_ADMIN for /admin* -->
                <rule path="^/admin" role="ROLE_ADMIN" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                // ...
                'default' => array(
                    // ...
                ),
            ),
           'access_control' => array(
               // require ROLE_ADMIN for /admin*
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
        ));

Jest to dobre dla zabezpieczania całych sekcji, ale zachodzi też potrzeba
:ref:`zabezpieczenia poszczególnych kontrolerów <book-security-securing-controller>`.

Można zdefiniować dowolną ilość wzorców URL, tak jak się chce - każdy z nich jest
wyrażeniem regularnym, ale tylko jeden z nich zostanie dopasowany. Symfony będzie
przeszukiwał je od początku do końca i zatrzma się, jak tylko znajdzie wpis
``access_control`` pasujący do ścieżki URL.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            access_control:
                - { path: ^/admin/users, roles: ROLE_SUPER_ADMIN }
                - { path: ^/admin, roles: ROLE_ADMIN }

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

                <rule path="^/admin/users" role="ROLE_SUPER_ADMIN" />
                <rule path="^/admin" role="ROLE_ADMIN" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'access_control' => array(
                array('path' => '^/admin/users', 'role' => 'ROLE_SUPER_ADMIN'),
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
        ));

Poprzedzenie ścieżki znakiem ``^`` oznacza, że dopasowywane będą tylko ścieżki
URL *rozpoczynajace* się od ciągu takiego jak wzorzec. Na przykład, wzorzec ``/admin``
(bez znaku ``^``) będzie pasować do ``/admin/foo`` ale także dopasuje ścieżki URL
takie jak ``/foo/admin``.

.. _security-book-access-control-explanation:

.. sidebar:: Konieczność zrozumienia jak działa sekcja ``access_control``

    Wpisy sekcji ``access_control`` są bardzo silne, ale mogą też być niebezpieczne
    (ponieważ dotyczą bezpieczeństwa), jeśli nie rozumie się jak to działa.
    Oprócz ścieżki URL, wpisy ``access_control`` mogą dopasowywać adres IP,
    nazwę hosta i metody HTTP. Wpisy te mogą być też używane do przekierowywania
    użytkownika do wersji ``https`` wzorca URL.

    Więcej na ten temat można znależć w artykule :doc:`/cookbook/security/access_control`.

.. _`book-security-securing-controller`:

Zabezpieczanie kontrolerów i innego kodu
........................................

Można łatwo zablokować dostęp do akcji kontrolera::

    // ...

    public function helloAction($name)
    {
        // Drugi parametr używany jest do określenia tego, jaki obiekt z daną rolą jest sprawdzany.
        $this->denyAccessUnlessGranted('ROLE_ADMIN', null, 'Unable to access this page!');

        // Old way :
        // if (false === $this->get('security.authorization_checker')->isGranted('ROLE_ADMIN')) {
        //     throw $this->createAccessDeniedException('Unable to access this page!');
        // }

        // ...
    }

.. versionadded:: 2.6
    Metoda ``denyAccessUnlessGranted()`` została wprowadzona w Symfony 2.6. Poprzednio
    (i jeszcze nadal), można sprawdzać dostęp bezpośrednio i zrzucać ``AccessDeniedException``,
    tak jak pokazano to w powyższym przykładzie).

.. versionadded:: 2.6
    Usługa ``security.authorization_checker`` została wprowadzona w Symfony 2.6. Wcześnie
    trzeba było uzywać metody ``isGranted()`` usługi ``security.context``.

W obu przypadkach zrzucany jest specjalny wyjątek
:class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException`,
co w efekcie wywołuje wewnątrz Symfony odpowiedź 403 HTTP.

Jeśli użytkownik nie jest jeszcze zalogowany, zostanie poproszony o zalogowanie się
(czyli przekierowany do strony logowania). Jeeśli użytkownik jest zalogowany, ale
nie ma roli ``ROLE_ADMIN``, to zostanie wyświetlona strona *403 access denied*
(którą można :ref:`odpowiednio dostosować <cookbook-error-pages-by-status-code>`).
Jeśli użytkownik jest zalogowany i posiada właściwą rolę, to nastąpi wykonanie
kodu.

.. _book-security-securing-controller-annotations:

Przy zastosowaniu SensioFrameworkExtraBundle, można również zabezpieczyć kontroler
używając adnotacji::

    // ...
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;

    /**
     * @Security("has_role('ROLE_ADMIN')")
     */
    public function helloAction($name)
    {
        // ...
    }

Więcej informacji na ten temat można znaleźć w `dokumentacji FrameworkExtraBundle`_.

.. _book-security-template:

Kontrola dostępu w szablonach
.............................

Jeśli chce się sprawdzić w szablonie, czy bieżący użytkownik posiada określoną
rolę, trzeba użyć wbudowanej funkcji pomocniczej:

.. configuration-block::

    .. code-block:: html+jinja

        {% if is_granted('ROLE_ADMIN') %}
            <a href="...">Delete</a>
        {% endif %}

    .. code-block:: html+php

        <?php if ($view['security']->isGranted('ROLE_ADMIN')): ?>
            <a href="...">Delete</a>
        <?php endif ?>

Jeśli używa się tej funkcji i nie jest się za zaporą, to zostanie zrzucony wyjątek.
Tak więc, zawsze dobrym pomysłem jest, aby mieć główna zaporę, która obejmuje wszystkie
ścieżki URL (tak jak pokazano to wcześniej w tym rozdziale).

.. caution::

    Trzeba być bardzo ostrożnym z używaniem tej funkcji w bazowym ukladzie strony
    lub na stronach błędów! Z powodu kilku wewnętrznych szczegółów Symfony, trzeba
    unikać załamywanie się stron błędów w środowisku ``prod``, opakowując wywołania
    w tych szablonach w kod sprawdzający dla ``app.user``:

    .. code-block:: html+jinja

        {% if app.user and is_granted('ROLE_ADMIN') %}

Zabezpieczenie innych usług
...........................

W Symfony może być chronione wszystko przez wykonanie czegośc podobnego do kodu
użytego przez nas do zabezpieczenia akcji kontrolera. Przyjmijmy na przykład, że
mamy jakąś usługę (czyli klasę PHP), której zadaniem jest wysyłanie wiadomości
email. Można ograniczyć korzystanie z tej klasy do określonych użytkowników,
bez względu na to, gdzie jest ona wykorzytywana.

Więcej informacji można znaleźć w artykule :doc:`/cookbook/security/securing_services`.

Sprawdzanie, czy użytkownik jest zalogowany (IS_AUTHENTICATED_FULLY)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jak dotąd, sprawdzaliśmy dostęp w oparciu o role - ciągi te rozpoczynają się od
``ROLE_`` i są przypisywane do użytkowników. Lecz jeśli chcesz *tylko* sprawdzić,
czy użytkownik jest zalogowany (bez sprawdzania roli), to można użyć w metody
``isGranted`` z argumentem ``IS_AUTHENTICATED_FULLY``::

    // ...

    public function helloAction($name)
    {
        if (!$this->get('security.authorization_checker')->isGranted('IS_AUTHENTICATED_FULLY')) {
            throw $this->createAccessDeniedException();
        }

        // ...
    }

.. tip::

    Można oczywiście korzystać z tego też w ``access_control``.

``IS_AUTHENTICATED_FULLY`` nie jest rola, ale niby działa tak jak rola i każdy
użytkownik, który się zalogował będzie miał przypisany ten atrybut. W rzeczywistości
istnieją trzy specjalne atrybuty, takie jak ten:

* ``IS_AUTHENTICATED_REMEMBERED``: Atrybut ten posiadają *wszyscy* zalogowani
  użytkownicy, nawet jeśli są zalogowani z powodu "remember me cookie". Można to
  też uzywać do sprawdzeniam, czy użytkownik jest zalogowany, nawet gdy nie używa
  się :doc:`funkcjonalności "remember me" </cookbook/security/remember_me>`.

* ``IS_AUTHENTICATED_FULLY``: Jest to podobne do ``IS_AUTHENTICATED_REMEMBERED``,
  ale silniejsze. Uzytkownicy, którzy są zalogowani z powodu "remember me cookie"
  beda mieć atrybut ``IS_AUTHENTICATED_REMEMBERED``, ale nie ``IS_AUTHENTICATED_FULLY``.

* ``IS_AUTHENTICATED_ANONYMOUSLY``: Atrybut ten posiadaja *wszyscy* uzytkownicy
  (nawet ci anonimowi) - jest on przydatny, gdy dostęp jest zapewniany z adresów
  *whitelisting* - pewne szczegóły są omówione w :doc:`/cookbook/security/access_control`.

.. _book-security-template-expression:

Można te używać wyrażenia wewnątrz szablonów:

.. configuration-block::

    .. code-block:: html+jinja

        {% if is_granted(expression(
            '"ROLE_ADMIN" in roles or (user and user.isSuperAdmin())'
        )) %}
            <a href="...">Delete</a>
        {% endif %}

    .. code-block:: html+php

        <?php if ($view['security']->isGranted(new Expression(
            '"ROLE_ADMIN" in roles or (user and user.isSuperAdmin())'
        ))): ?>
            <a href="...">Delete</a>
        <?php endif; ?>

Więcej szczegółów o wyrażeniach i bezpieczeństwie można znaleźć w rozdziale
:ref:`book-security-expressions`.

.. _security-secure-objects:

Listy kontroli dostępu (ACL): zabezpieczanie poszczególnych obiektów bazy danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Wyobraź sobie, że projktujesz blog, gdzie użytkownicy mogą komentować wpisy.
Chcesz też, aby użytkownik mógł edytować swoje komentarze, ale nie komentarze
innych użytkowników. Ponadto, Ty jako administrator, też chcesz mieć możliwość
edytowania *wszystkich* komentarzy.

Można to zrobić na dwa sposoby:

* :doc:`Używając tzw. wyborców (ang. voters) </cookbook/security/voters>` umożliwia
  napisanie własnej logiki biznesowej (np. użytkownik może edytować wpis, ponieważ
  jest twórcą) do ustalenia dostępu. Przypuszczalnie wybierzesz ten sposób, wystarczająca
  elastyczny, aby rozwązać powyższą sytuację.

* :doc:`Listy ACL </cookbook/security/acl>` pozwalaja utworzyć strukturę danych
  w bazie danych, gdzie pozna przypisać *każdemu* użytkownikowi *dowolny* dostęp
  (np. EDIT, VIEW) do *dowolnego* obiektu w aplikacji. Używaj tego, jeśli potrzebujesz,
  aby mógł przydzialać indywidualny dostęp w całej aplikacji poprzez jakiś interfejs
  administracyjny.

W obu przypadkach, nadal można przydzialać prawa dostępu, tak jak omówiono to
w poprzednich rozdziałach.

.. index::
   obiekt użytkownika

Pobieranie obiektu użytkownika
------------------------------

.. versionadded:: 2.6
     Usługa ``security.token_storage`` została wprowadzona w Symfony 2.6. Wcześniej
     trzeba było uzywac metody ``getToken()`` usługi ``security.context``.

Po uwierzytelnieniu użytkownika jest dostępny związany z nim obiekt ``User`` poprzez
usługę ``security.token_storage``. Od wnętrza akcji kontrolera wygląda to podobnie
do tego::

    public function indexAction()
    {
        if (!$this->get('security.authorization_checker')->isGranted('IS_AUTHENTICATED_FULLY')) {
            throw $this->createAccessDeniedException();
        }

        $user = $this->getUser();

        // the above is a shortcut for this
        $user = $this->get('security.token_storage')->getToken()->getUser();
    }

.. tip::

    Użytkownik będzie obiektem a jego klasa będzie zależeć od stosowanego 
    :ref:`dostawcy użytkownika <security-user-providers>`.

Teraz można wywoływać metody obiektu User. Na przykład, jeśli obiekt User ma metodę
``getFirstName()``, można zastosować coś takiego::

    use Symfony\Component\HttpFoundation\Response;
    // ...

    public function indexAction()
    {
        // ...

        return new Response('Well hi there '.$user->getFirstName());
    }

Zawsze należy sprawdzać, czy użytkownik jest zalogowany
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jest bardzo ważne, aby najpierw sprawdzić to, czy użytkownik jest uwierzytelniony.
Jeśli nie jest, to ``$user`` będzie wartością ``null`` lub łańcuchem ``anon.``.
Zaraz, zaraz, ``anan``? Tak, nie jest to takie dziwne! Jeśli nikt nie jest zalogowany,
to użytkownik odwiedzający jest technicznie użytkownikiem anonimowym a metoda
``getUser()`` zwraca dla wygody wartość ``null``.

Wskazówka jest taka: przed użyciem obiektu User zawsze sprawdzaj, czy użytkownik
jest zalogowany i używaj do tego metody ``isGranted``
(lub :ref:`access_control <security-authorization-access-control>`)::

    // Do sprawdzania, czy uzytkownik jest zalogowany, uzywaj tego
    if (!$this->get('security.authorization_checker')->isGranted('IS_AUTHENTICATED_FULLY')) {
        throw $this->createAccessDeniedException();
    }

    // Nigdy nie używaj obiektu User do sprawdzenia zalogowania się uzytkownika
    if ($this->getUser()) {

    }

Pobieranie uzytkownika w szablonie
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W szablonie Twig obiekt ten może być dostępny poprzez klucz :ref:`app.user <reference-twig-global-app>`:

.. configuration-block::

    .. code-block:: html+jinja

        {% if is_granted('IS_AUTHENTICATED_FULLY') %}
            <p>Username: {{ app.user.username }}</p>
        {% endif %}

    .. code-block:: html+php

        <?php if ($view['security']->isGranted('IS_AUTHENTICATED_FULLY')): ?>
            <p>Username: <?php echo $app->getUser()->getUsername() ?></p>
        <?php endif; ?>

.. index::
   wylogowanie

.. _book-security-logging-out:

Wylogowanie
-----------

Zwykle, oprócz logowania trzeba umożliwić wylogowanie użytkownika. Na szczęście,
zapora może to obsługiwać automatycznie, jeśli w konfiguracji ustawi się odpowiednio
parametr ``logout``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    # ...
                    logout:
                        path:   /logout
                        target: /

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

                <firewall name="secured_area">
                    <!-- ... -->
                    <logout path="/logout" target="/" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'secured_area' => array(
                    // ...
                    'logout' => array('path' => '/logout', 'target' => '/'),
                ),
            ),
        ));

Następnie musi się utworzyć trasę dla tej ścieżki URL (ale nie akję):

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        logout:
            path: /logout

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="logout" path="/logout" />
        </routes>

    ..  code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('logout', new Route('/logout'));

        return $collection;

Gotowe! Odsyłając użytkownika do ``/logout`` (albo tam, gdzie skonfigurowało się
``path``), Symfony wyloguje bieżącego użytkownika.

Po wylogowaniu, użytkownik zostanie przekierowany na ścieżkę określoną w parametrze
``target`` (np. do ``homepage``, jak w naszym przykładzie).

.. tip::

    Jeśli chce się wykonać po wylogowaniu jakąś specjalną procedurę, można określíć
    handler powodzenia wylogowania, dodając klucza ``success_handler`` i wskazując 
    go jako identyfikator klasy implementującej tą procedurę
    :class:`Symfony\\Component\\Security\\Http\\Logout\\LogoutSuccessHandlerInterface`.
    Zobacz do :doc:`Informatora konfiguracji bezpieczeństwa </reference/configuration/security>`.

.. index::
   szyfrowanie hasła

.. _`security-encoding-password`:

Dynamiczne kodowanie hasła
--------------------------

Jeśli, na przykład, przechowuje się uzytkowników w bazie danych, trzeba zakodować
hasła użytkowników przed ich wstawieniem do bazy danych. Bez względu na używany
algorytm szyfrowania hasła dla obiektu użytkownika, zakodowane hasło może być
zawsze określone w następujący sposób::

    // czym kolwiek nie jest obiekt User
    $user = new AppBundle\Entity\User();
    $plainPassword = 'ryanpass';
    $encoder = $this->container->get('security.password_encoder');
    $encoded = $encoder->encodePassword($user, $plainPassword);

    $user->setPassword($encoded);

.. versionadded:: 2.6
    Usługa ``security.password_encoder`` została wprowadzona w Symfony 2.6.

Do wykonywania tego kodu niezbędne jest posiadanie odpowieniego kodera dla klasy
użytkownika (np. ``AppBundle\Entity\User`` )  skonfigurowanego w kluczu ``encoders``
w ``app/config/security.yml``.

Obiekt ``$encoder`` ma też metodę ``isPasswordValid``, która pobiera obiekt
``User`` jako pierwszy argument i zwykłe hasło do sprawdzenia jako drugi argument.

.. caution::

    Gdy zezwala się na wysyłanie przez uzytkowników haseł w zwykłym tekście
    (np. formularz rejestracyjny, formularz zmiany hasła), trzeba zastosować walidację
    gwarantujaca, że hasło ma 4096 znaków lub ,niej. Więcej szczegółów znajduje się
    w artykule :ref:`Jak zaimplementować prosty formularz rejestracyjny <cookbook-registration-password-max>`.

.. index:
   single: role; hierarchia

.. _security-role-hierarchy:

Role hierarchiczne
------------------

Zamiast kojarzyć z użytkownikiem wiele ról, można zdefiniować zasady dziedziczenia
rół, tworząc hierarchie ról:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            role_hierarchy:
                ROLE_ADMIN:       ROLE_USER
                ROLE_SUPER_ADMIN: [ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH]

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

                <role id="ROLE_ADMIN">ROLE_USER</role>
                <role id="ROLE_SUPER_ADMIN">ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH</role>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'role_hierarchy' => array(
                'ROLE_ADMIN'       => 'ROLE_USER',
                'ROLE_SUPER_ADMIN' => array(
                    'ROLE_ADMIN',
                    'ROLE_ALLOWED_TO_SWITCH',
                ),
            ),
        ));

Z powyższą konfiguracją użytkownicy z rolą ``ROLE_ADMIN`` będą też mieli rolę
``ROLE_USER``. Rola ``ROLE_SUPER_ADMIN`` ma role ``ROLE_ADMIN``, ``ROLE_ALLOWED_TO_SWITCH``
i ``ROLE_USER`` (dziedziczoną z ``ROLE_ADMIN``).

Uwierzytelnianie bezstanowe
---------------------------

Domyślnie Symfony bazuje na ciasteczku sesji do zapewniania kontekstu bezpieczeństwa
użytkownika. Jeśli jednak używa się dla instancji certyfikatów lub uwierzytelniania
HTTP, utrwalanie nie jest potrzebne, ponieważ poswiadczenia są dostępne dla każdego
żądania. W tym przypadku i jeśli nie ma potrzeby przechowywania niczego innego
między żądaniami, można aktywować uwierzytelnianie bezstanowe (co oznacza, że
żadne ciasteczko nie zostanie utworzone przez Symfony):

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                main:
                    http_basic: ~
                    stateless:  true

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

                <firewall name="main" stateless="true">
                    <http-basic />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main' => array('http_basic' => null, 'stateless' => true),
            ),
        ));

.. note::

    W przypadku korzystania z logowania formularzowego, Symfony będzie tworzyć
    ciasteczka, nawet jeśli ``stateless`` ustawi się na ``true``.

.. _book-security-checking-vulnerabilities:

Sparawdzanie znanych luk bezpieczeństwa z zależnościach
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Podczas używania wielu zależności w projekcie Symfony, kilka z nich może zawierać
luki bezpieczeństwa. Dlatego Symfony zawiera polecenie o nazwie ``security:check``,
które sprawdza plik ``composer.lock`` do znalezienia znanych luk bezpieczeństwa
w zainstalowanych zależnościach:

.. code-block:: bash

    $ php app/console security:check

Dobrą praktyką jest regularne wykonywanie tego polecenia, tak aby móc aktualizować
 lub wymieniać zagrożone zależności tak szybko jak to jest możliwe. Wewnetrznie,
polecenie to używa publicznej `bazy danych biuletynów zabeczeń`_ publikowanych
przez organizację FriendsOfPHP.

.. tip::

    Polecenie ``security:check`` kończy się nie zerowym kodem wyjścia, jeśli
    jakakolwiek z zależności ma luke bezpieczeństwa. Dlatego łatwo to polecenie
    zintegrować z procesem kompilacji.

Wnioski końcowe
---------------

Teraz znasz juz trochę więcej niż tylko podstawy bezpieczeństwa. Nie omówilismy
tu trudniejszych zagadnień związanych z bezpieczeństwem, które będzie się potrzebowało
w przypadku indywidualnych wymagań, takich jak własna strategia uwierzytelniania
(np. tokeny API), złożona ligika autoryzacji i wiele innych rzeczy (ponieważ
tematyka bezpieczeństwa jest skomplikowana!).

Na szczęście, istnieje
:doc:`działt Receptariusza poświecony bezpieczeństwu </cookbook/security/index>`,
gdzie można znaleźć artykuły poruszające zagadnienia nie omówione przez nas.
Można też skorzystać z rozdziału :doc:`Informator bezpieczeństwa </reference/configuration/security>`.

Dalsza lektura
--------------

* :doc:`Wymuszanie HTTP/HTTPS </cookbook/security/force_https>`
* :doc:`Jak podszywać sie pod uzytkownika? </cookbook/security/impersonating_user>`
* :doc:`/cookbook/security/voters`
* :doc:`Listy kontroli dostęþu (ACL) </cookbook/security/acl>`
* :doc:`/cookbook/security/remember_me`
* :doc:`/cookbook/security/multiple_user_providers`

.. _`narzędzie online`: https://www.dailycred.com/blog/12/bcrypt-calculator
.. _`dokumentacji FrameworkExtraBundle`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/index.html
.. _`bazy danych biuletynów zabeczeń`: https://github.com/FriendsOfPHP/security-advisories
.. _`HWIOAuthBundle`: https://github.com/hwi/HWIOAuthBundle
