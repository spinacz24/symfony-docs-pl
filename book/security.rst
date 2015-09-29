.. highlight:: php
   :linenothreshold: 2

.. index::
   ochrona dostępu, system bezpieczeństwa, bezpieczeństwo

System bezpieczeństwa
=====================

Ochrona dostępu jest dwuetapowym procesem którego celem jest uniemożliwienie
użytkownikowi dostępu do zasobów do których nie powinien mieć dostępu.

W pierwszym etapie tego procesu system bezpieczeństwa identyfikuje, kto jest użytkownikiem,
poprzez żądanie od użytkownika przesłania jakiegoś rodzaju identyfikacji. Jest to
nazywane **uwierzytelnianiem** (*ang. authentication*), a to oznacza, że system
próbuje się dowiedzieć, kim jest osoba starająca się uzyskać dostęp do zasobów.

Gdy system już się dowie kim jest osoba starająca się uzyskać dostęp do zasobów,
to następnym etapem jest ustalenie, czy może mieć ona dostęp do danego zasobu.
Ta część procesu nosi nazwę **autoryzacji** (*ang. authorization*) co oznacza,
że system sprawdza, czy ta osoba ma uprawnienia do wykonania określonej czynności.

.. image:: /images/book/security_authentication_authorization.png
   :align: center

Ponieważ najlepszym sposobem nauki jest przeanalizowanie przykładu, to zaczniemy
od zabezpieczenia aplikacji podstawowym uwierzytelnianiem HTTP.

.. note::

    Dostępny jest `komponent bezpieczeństwa Symfony`_, będący samodzielną biblioteką
    PHP, która może być zastosowana wewnątrz dowolnego projektu PHP.

.. index::
   single: bezpieczeństwo; uwierzytelnianie
   single: uwierzytelnianie
   
Prosty przykład: uwierzytelnianie HTTP
--------------------------------------

Komponent bezpieczeństwa może zostać skonfigurowany za pośrednictwem konfiguracji
aplikacji. W rzeczywistości większość standardowych ustawień bezpieczeństwa,
to tylko kwestia zastosowania właściwej konfiguracji. Poniższa konfiguracja powiadamia
Symfony aby zabezpieczyło każdy adres URL pasujący do wzorca ``/admin/*`` i prosiło
użytkownika o podanie poświadczenia, stosując podstawowe uwierzytelnianie HTTP
(tj. przez zastosowanie starej szkoły pól logowania: nazwy użytkownika i hasła):

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    pattern:   ^/
                    anonymous: ~
                    http_basic:
                        realm: "Secured Demo Area"

            access_control:
                - { path: ^/admin, roles: ROLE_ADMIN }

            providers:
                in_memory:
                    memory:
                        users:
                            ryan:  { password: ryanpass, roles: 'ROLE_USER' }
                            admin: { password: kitten, roles: 'ROLE_ADMIN' }

            encoders:
                Symfony\Component\Security\Core\User\User: plaintext

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8"?>

        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <!-- app/config/security.xml -->

            <config>
                <firewall name="secured_area" pattern="^/">
                    <anonymous />
                    <http-basic realm="Secured Demo Area" />
                </firewall>

                <access-control>
                    <rule path="^/admin" role="ROLE_ADMIN" />
                </access-control>

                <provider name="in_memory">
                    <memory>
                        <user name="ryan" password="ryanpass" roles="ROLE_USER" />
                        <user name="admin" password="kitten" roles="ROLE_ADMIN" />
                    </memory>
                </provider>

                <encoder class="Symfony\Component\Security\Core\User\User" algorithm="plaintext" />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'pattern'    => '^/',
                    'anonymous'  => array(),
                    'http_basic' => array(
                        'realm'  => 'Secured Demo Area',
                    ),
                ),
            ),
            'access_control' => array(
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
            'providers' => array(
                'in_memory' => array(
                    'memory' => array(
                        'users' => array(
                            'ryan' => array('password' => 'ryanpass', 'roles' => 'ROLE_USER'),
                            'admin' => array('password' => 'kitten', 'roles' => 'ROLE_ADMIN'),
                        ),
                    ),
                ),
            ),
            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => 'plaintext',
            ),
        ));

.. tip::

    Standardowa dystrybucja Symfony oddziela konfigurację bezpieczeństwa
    do osobnego pliku (np. ``app/config/security.yml``). Jeśli nie chce się mieć
    odrębnego pliku konfiguracji bezpieczeństwa, to można umieścić tą konfigurację
    bezpośrednio w głównym pliku konfiguracyjnym (np. ``app/config/config.yml``).

Rezultatem tej konfiguracji jest w pełni funkcjonalny system zabezpieczeń, wyglądający
następująco:

* Istnieje dwóch użytkowników systemowych (``ryan`` i ``admin``);
* Użytkownicy uwierzytelniają się poprzez monit podstawowego uwierzytelniania HTTP;
* Wszystkie adresy URL pasujące do wzorca ``/admin/*`` są zabezpieczone i tylko
  użtkownik ``admin`` może mieć do nich dostęp;
* Wszystkie adresy URL nie pasujące do wzorca ``/admin/*`` są dostępne dla wszystkich
  użytkowników (i użytkowników niezalogowanych).

Przyjrzyjmy się pokrótce jak działa system bezpieczeństwa i jak spełnia swoją rolę
każda część konfiguracji.

Jak działają zabezpieczenia: uwierzytelnianie i autoryzacja
-----------------------------------------------------------

System bezpieczeństwa działa przez określenie kim jest użytkownik (tj. uwierzytelnianie)
i następnie sprawdzenie, czy ten użytkownik powinien mieć dostęp do określonego zasobu
lub adresu URL.


.. index::
   single: bezpieczeństwo; uwierzytelnianie
   single: bezpieczeństwo; zapory
   single: uwierzylenianie; zapory

.. _book-security-firewalls:

Zapory (Uwierzytenianie)
~~~~~~~~~~~~~~~~~~~~~~~~

Gdy użytkownik wysyła żądanie na adres URL, który jest chroniony przez zaporę,
aktywowany zostaje system bezpieczeństwa. Zadaniem zapory jest ustalenie, czy
użytkownik musi być uwierzytelniony i jeśli to zrobi, to odesłanie użytkownikowi
odpowiedzi inicjującej proces uwierzytelniania.

Zapora jest aktywowana, gdy adres URL przychodzącego żądania dopasowuje wartość
wyrażenia regularnego wzorca zapory. W tym przykładzie, wzorzec (``^/``) będzie
dopasowywał każde przychodzące żądanie. Fakt, że zapora jest aktywowana nie oznacza,
że pola nazwy użytkownika i hasła uwierzytelniania HTTP są zawsze wyświetlane.
Na przykład, jakikolwiek użytkownik może uzyskać dostęp do ``/foo`` bez monitowania
o uwierzytelnienie.

.. image:: /images/book/security_anonymous_user_access.png
   :align: center

Działa to po pierwsze dlatego, że zapora dopuszcza *użytkowników anonimowych* zgodnie
z parametrem konfiguracyjnym ``anonymous``. Innymi słowami, zapora nie wymaga od
użytkownika niezwłocznego pełnego uwierzytelnienia. Ponieważ nie jest wymagana
szczególna rola dla dostępu do ``/foo`` (w ramach sekcji ``access_control``),
żądanie może być zaakceptowane bez jakiegokolwiek pytania użytkownika o uwierzytelnienie.

Jeśli usunie się klucz ``anonymous``, to zapora będzie zawsze niezwłocznie przeprowadzać
pełne uwierzytelnianie użytkownika.

.. index::
   single: bezpieczeństwo; kontrola dostępu
   single: bezpieczeństwo; autoryzacja
   single: autoryzacja

Kontrola dostępu (Autoryzacja)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli użytkownik żąda dostępu do ``/admin/foo``, to jednak proces zachowuje się
inaczej. Dzieje się tak, ponieważ sekcja konfiguracji access_control określa, że
każdy adres URL dopasowany do wzorca wyrażenia regularnego ``^/admin``
(np. ``/admin`` lub cokolwiek pasującego do ``/admin/*``) wymaga roli ``ROLE_ADMIN``.
Role są podstawą większości autoryzacji: użytkownik może uzyskać dostęp do ``/admin/foo``
tylko jeśli posiada rolę ``ROLE_ADMIN``.

.. image:: /images/book/security_anonymous_user_denied_authorization.png
   :align: center

Podobnie jak wcześniej, gdy użytkownik pierwotnie wysyła żądanie, zapora nie prosi
o jakąś identyfikację. Jednak gdy tylko warstwa kontroli dostępu zabroni użytkownikowi
dostępu (ponieważ anonimowy użytkownik nie posiada roli ``ROLE_ADMIN``), zapora
przystępuje do działania i rozpoczyna proces uwierzytelniania. Proces uwierzytelniania
zależy od mechanizmów uwierzytelniania, jakie się stosuje. Na przykład, gdy stosuje
się metody uwierzytelniania poprzez logowanie formularzowe, użytkownik będzie
przekierowywany do strony logowania. Jeśli stosuje się uwierzytelnianie HTTP,
użytkownikowi zostanie przesłana odpowiedź HTTP 401, więc użytkownik zobaczy pola
nazwy użytkownik i hasła.

Użytkownik ma teraz możliwość przesłania z powrotem do aplikacji swoich danych
indentyfikacyjnych. Jeśli są one prawidłowe, to pierwotne żądanie zostaje ponownie
przebadane.

.. image:: /images/book/security_ryan_no_role_admin_access.png
   :align: center

W tym przykładzie użytkownik ``ryan`` zostaje uwierzytelniony pomyślnie przez zaporę.
Lecz ponieważ ryan nie ma roli ``ROLE_ADMIN``, to znowuż otrzymuje odmowę dostępu
do zasobu ``/admin/foo``. Ostatecznie użytkownik zobaczy jakiś komunikat z informacją
o zablokowaniu dostępu.

.. tip::

    Gdy Symfony zabranie użytkownikowi dostępu do zasobu, użytkownik widzi ekran
    błędu i otrzymuje odpowiedź HTTP ze statusem błędu 403 (``Forbidden``).
    Można dostosować ekran błędu zakazu dostępu, postępując zgodnie ze wskazówki
    podanymi w artykule :ref:`Strony błędów<cookbook-error-pages-by-status-code>`.

Na koniec, jeśli użytkownik ``admin`` zażąda zasobu ``/admin/foo`` ma miejsce podobny
proces, ale teraz po uwierzytelnieniu warstwa kontroli dostępu zezwoli na zrealizowanie
przychodzącego żądania:

.. image:: /images/book/security_admin_role_access.png
   :align: center

Gdy użytkownik żąda chronionego zasobu, to przepływ żądania jest prosty, ale bardzo
elastyczny. Jak zobaczymy później, uwierzytelnianie może być obsługiwane na wiele
sposobów, włączając w to logowanie formularzowe, certyfikat X.509 lub za pomocą
uwierzytelniania użytkownika poprzez Twitter. Niezależnie od metody, przepływ
uwierzytelniania jest zawsze taki sam:

#. Użytkownik uzyskuje dostęp do chronionych zasobów;
#. Aplikacja przekierowuje użytkownika do formularza logowania;
#. Użytkownik przesyła swoje poświadczenie (np. nazwę użytkownika i hasło);
#. Zapora uwierzytelnia użytkownika;
#. Dla uwierzytelnionego użytkownika realizowane jest ponownie pierwotne żądania.

.. note::

    W rzeczywistości dokładny przebieg procesu zależy trochę od zastosowanego
    mechanizmu uwierzytelniania. Na przykład, gdy stosuje się logowanie formularzowe,
    użytkownik przesyła swoje poświadczenie na adres URL przetwarzający formularz
    (np. ``/login_check``) i następnie jest przekierowywany na pierwotnie żądany
    adres URL (np. ``/admin/foo``). Lecz z uwierzytelnianiem HTTP użytkownik przesyła
    poświadczenie bezpośrednio na oryginalny adres URL (np.``/admin/foo``) i następnie
    użytkownikowi zwracana jest strona w tym samym żądaniu (tj. bez przekierowania).
    
    Ten rodzaj dziwactw nie powinny sprawiać żadnych problemów, ale dobrze jest
    o tym pamietać.

.. tip::

    Nauczysz się później, jak można w Symfony2 zabezpieczać cokolwiek, w tym
    konkretne kontrolery, obiekty lub nawet metody PHP.


.. index::
   single: bezpieczeństwo; logowanie formularzowe
   single: logowanie
   single: uwierzytelnianie; logowanie

.. _book-security-form-login:

Stosowanie tradycyjnego logowania formularzowego
------------------------------------------------

.. tip::

    W tym rozdziale dowiesz się jak utworzyć podstawowy formularz logowania, który
    obsługuje tylko użytkowników określonych sztywno w programie, a ściślej w pliku
    ``security.yml``, ``security.xml`` czy ``security.php``.

    Aby się dowiedzieć jak załadować użytkowników z bazy danych, proszę przeczytać
    artykuł :doc:`Jak załadować użytkowników systemu bezpieczeństwa z bazy danych (dostawca encji)
    </cookbook/security/entity_provider>`. Posługując się
    wiedzą z tego artykułu i niniejszego rozdziału, można stworzyć pełny system
    formularza logowania wykorzystującego dane z bazy danych.

Poprzednio poznaliśmy, jak ukryć aplikację za zaporą i następnie zabezpieczyć
dostęp do określonych obszarów aplikacji poprzez role. Używając uwierzytelniania
HTTP można z łatwością dopasowywać się do natywnych pól ``username`` i ``password``
oferowanych przez przeglądarki. Ale Symfony obsługuje wiele mechanizmów uwierzytelniania.
Więcej szczegółów na ten temat można znaleźć w dokumencie
:doc:`Informacje o konfiguracji bezpieczeństwa</reference/configuration/security>`.
 
W tym rozdziale wzbogacimy ten proces, umożliwiając użytkownikowi uwierzytelnianie
się poprzez tradycyjny formularz logowania HTML.

Po pierwsze, włączymy logowanie formularzowe kontrolowane przez zaporę:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    pattern:   ^/
                    anonymous: ~
                    form_login:
                        login_path: login
                        check_path: login_check

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8"?>

        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <!-- app/config/security.xml -->

            <config>
                <firewall name="secured_area" pattern="^/">
                    <anonymous />
                    <form-login login-path="login" check-path="login_check" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'pattern'    => '^/',
                    'anonymous'  => array(),
                    'form_login' => array(
                        'login_path' => 'login',
                        'check_path' => 'login_check',
                    ),
                ),
            ),
        ));

.. tip::

    Jeśli nie musi się dostosowywać wartości ``login_path`` lub ``check_path``
    (wartości tutaj używane są wartościami domyślnymi), to można skrócić swoją
    konfigurację:

    .. configuration-block::

        .. code-block:: yaml
           
            form_login: ~

        .. code-block:: xml

            <form-login />

        .. code-block:: php

            'form_login' => array(),

Teraz, gdy system zabezpieczeń inicjuje proces uwierzytelniania, to będzie przekierowywać
użytkownika do formularza logowania(domyślnie ``/login``). Zaimplementowanie tego
formularza jest już Twoim zadaniem. Najpierw utwórzmy dwie trasy: jedną wyświetlającą
formularz logowania (np. ``/login``) i drugi obsługujący zgłoszenie formularza logowania
(np. ``/login_check``):

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        login:
            pattern:   /login
            defaults:  { _controller: AcmeSecurityBundle:Security:login }
        login_check:
            pattern:   /login_check

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="login" pattern="/login">
                <default key="_controller">AcmeSecurityBundle:Security:login</default>
            </route>
            <route id="login_check" pattern="/login_check" />

        </routes>

    ..  code-block:: php
        :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('login', new Route('/login', array(
            '_controller' => 'AcmeDemoBundle:Security:login',
        )));
        $collection->add('login_check', new Route('/login_check', array()));

        return $collection;

.. note::

    Nie potrzeba implementować kontrolera dla adresu URL ``/login_check``, jako że
    zapora będzie automatycznie przechwytywać i przetwarzać każde zgłoszenie formularza
    kierowane na ten adres URL. Jednak *musi się* mieć trasę dla tego URL (jak
    pokazano tutaj), a także jeszcze jedną dla ścieżki wylogowania (zobacz
    :ref:`book-security-logging-out`).

Proszę zauważyć, że nazwa trasy logowania nie jest istotna. To co jest istotne,
to adres URL trasy (``/login``) dopasowujący wartość konfiguracyjną ``login_path``,
ponieważ system bezpieczeństwa będzie przekierowywał tam użytkowników chcących się
zalogować.

Następnie trzeba stworzyć kontroler, który będzie wyświetlał formularz logowania::

    // src/Acme/SecurityBundle/Controller/SecurityController.php;
    namespace Acme\SecurityBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Core\SecurityContextInterface;

    class SecurityController extends Controller
    class SecurityController extends Controller
    {
        public function loginAction(Request $request)
        {
            $session = $request->getSession();

            // get the login error if there is one
            if ($request->attributes->has(SecurityContextInterface::AUTHENTICATION_ERROR)) {
                $error = $request->attributes->get(
                    SecurityContextInterface::AUTHENTICATION_ERROR
                );
            } elseif (null !== $session && $session->has(SecurityContextInterface::AUTHENTICATION_ERROR)) {
                $error = $session->get(SecurityContextInterface::AUTHENTICATION_ERROR);
                $session->remove(SecurityContextInterface::AUTHENTICATION_ERROR);
            } else {
                $error = '';
            }

            // last username entered by the user
            $lastUsername = (null === $session) ? '' : $session->get(SecurityContextInterface::LAST_USERNAME);

            return $this->render(
                'AcmeSecurityBundle:Security:login.html.twig',
                array(
                    // last username entered by the user
                    'last_username' => $lastUsername,
                    'error'         => $error,
                )
            );
        }
    }

Nie pozwól aby ten kontroler się mylił. Jak zobaczymy za chwilę, gdy użytkownik
zgłasza formularz, system bezpieczeństwa automatycznie obsługuje zgłoszenie formularza.
Jeśli użytkownik zgłosił nieprawidłową nazwę użytkownika lub hasło, to kontroler
odczytuje błąd zgłoszenia formularza z systemu bezpieczeństwa, tak że może on być
wyświetlony z powrotem użytkownikowi.

Innymi słowami, Twoim zadaniem jest wyświetlenie formularza logowania i jakichkolwiek
błędów logowania, które mogą wystąpić, ale sprawdzeniem zgłoszonej nazwy użytkownika
i hasła zajmuje się już sam system bezpieczeństwa.

Na koniec, stworzymy odpowiedni szablon.

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        {# src/Acme/SecurityBundle/Resources/views/Security/login.html.twig #}
        {% if error %}
            <div>{{ error.message }}</div>
        {% endif %}

        <form action="{{ path('login_check') }}" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="{{ last_username }}" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            {#
                If you want to control the URL the user is redirected to on success (more details below)
                <input type="hidden" name="_target_path" value="/account" />
            #}

            <button type="submit">login</button>
        </form>

    .. code-block:: html+php
       :linenos:

        <?php // src/Acme/SecurityBundle/Resources/views/Security/login.html.php ?>
        <?php if ($error): ?>
            <div><?php echo $error->getMessage() ?></div>
        <?php endif; ?>

        <form action="<?php echo $view['router']->generate('login_check') ?>" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="<?php echo $last_username ?>" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <!--
                If you want to control the URL the user is redirected to on success (more details below)
                <input type="hidden" name="_target_path" value="/account" />
            -->

            <button type="submit">login</button>
        </form>

.. caution::

    Ten formularz logowania nie jest obecnie chroniony przed atakami CSRF. O tym
    jak można zabezpieczyć formularz logowania można przeczytać
    w :doc:`/cookbook/security/csrf_in_login_form`.

.. tip::

    Zmienna ``error`` przekazywana do szablonu jest instancją
    :class:`Symfony\\Component\\Security\\Core\\Exception\\AuthenticationException`.
    Może ona zawierać więcej informacji, nawet poufnych, o niepowodzeniu uwierzytelniania,
    więc stosuj ją mądrze!

Formularz ma wiele wymagań. Po pierwsze, przez zgłoszenie formularza do ``/login_check``
(poprzez trasę ``login_check``), system zabezpieczeń przechwyci zgłoszony formularz
i przetworzy go automatycznie. Po drugie, system bezpieczeństwa oczekuje, że zgłoszone
pola będą nosić nazwę ``_username`` i ``_password`` (te nazwy pól mogą zostać
skonfigurowane).

I to jest to! Po zgłoszeniu formularza system bezpieczeństwa automatycznie sprawdza
poświadczenie użytkownika i albo uwierzytelnia użytkownika lub wysyła użytkownikowi
z powrotem formularz logowania, w którym może zostać wyświetlony komunikat o błędzie.

Przyjrzyjmy się całemu procesowi:

#. Użytkownik próbuje uzyskać dostęp do zasobu chronionego;
#. Zapora inicjuje automatycznie przetwarzanie poprzez przekierowanie użytkownika
   do formularza logowania (``/login``);
#. Strona ``/login`` renderuje formularz logowania wykorzystując trasę i kontroler,
   utworzone w poprzednim przykładzie;
#. Użytkownik zgłasza formularz logowania ``do /login_check``;
#. System bezpieczeństwa przechwytuje żądanie, sprawdza złożone poświadczenie,
   uwierzytelnia użytkownika, jeśli poświadczenie jest właściwe, a w przeciwnym
   przypadku wysyła z powrotem użytkownikowi formularz logowania.

Domyślnie, jeśli zgłoszone poświadczenie jest właściwe, to użytkownik zostanie
przekierowany do pierwotnie wywołanej strony (np. ``/admin/foo``). Jeśli użytkownik
na samym początku wywołał stronę logowania, to zostanie przekierowany do strony
początkowej. Może to zostać zmienione, umożliwiając przykładowo, przekierować
użytkownika na konkretny adres URL.

Więcej szczegółów o tym jak w ogóle dostosować proces logowania formularzowego
znajdziesz w artykule
:doc:`Jak dostosować formularz logowania</cookbook/security/form_login>`.

.. _book-security-common-pitfalls:

.. sidebar:: Unikanie typowych problemów

    Przy ustawianiu formularza logowania trzeba uważać na kilka typowych pułapek.

    **1. Utwórz poprawną trasę**

    Po pierwsze, upewnij się, że określenie tras dla adresów ``/login``
    i ``/login_check`` jest wykonane poprawnie i koresponduje z wartościami
    konfiguracyjnymi ``login_path`` i ``check_path``. Popełniony tu błąd w konfiguracji
    może skutkować przekierowywaniem na stronę 404 zamiast na stronę logowania lub
    zgłoszeniem formularza logowania, który nie działa (będzie można oglądać formularz
    logowania w kółko).

    **2. Upewnij się, że strona logowania nie jest chroniona**

    Upewnij się również, że strona logowania nie wymaga żadnych ról aby być wyświetloną.
    Na przykład, następująca konfiguracja, wymagająca roli ``ROLE_ADMIN`` dla wszystkich
    adresów URL (włączając w to adres ``/login``), spowoduje przekierowanie zapętlone:

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            access_control:
                - { path: ^/, roles: ROLE_ADMIN }

        .. code-block:: xml
           :linenos:

            <access-control>
                <rule path="^/" role="ROLE_ADMIN" />
            </access-control>

        .. code-block:: php
           :linenos:

            'access_control' => array(
                array('path' => '^/', 'role' => 'ROLE_ADMIN'),
            ),

    Usunięcie kontroli dostępu dla adresu ``/login`` rozwiązuje ten problem:

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            access_control:
                - { path: ^/login, roles: IS_AUTHENTICATED_ANONYMOUSLY }
                - { path: ^/, roles: ROLE_ADMIN }

        .. code-block:: xml
           :linenos:

            <access-control>
                <rule path="^/login" role="IS_AUTHENTICATED_ANONYMOUSLY" />
                <rule path="^/" role="ROLE_ADMIN" />
            </access-control>

        .. code-block:: php
           :linenos:

            'access_control' => array(
                array('path' => '^/login', 'role' => 'IS_AUTHENTICATED_ANONYMOUSLY'),
                array('path' => '^/', 'role' => 'ROLE_ADMIN'),
            ),

    Ponadto, jeśli zapora nie zezwala na dostęp użytkownikom anonimowym, potrzeba
    utworzyć specjalną zaporę, która umożliwi dostęp do strony logowania użytkownikom
    anonimowym:

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            firewalls:
                login_firewall:
                    pattern:    ^/login$
                    anonymous:  ~
                secured_area:
                    pattern:    ^/
                    form_login: ~

        .. code-block:: xml
           :linenos:

            <firewall name="login_firewall" pattern="^/login$">
                <anonymous />
            </firewall>
            <firewall name="secured_area" pattern="^/">
                <form_login />
            </firewall>

        .. code-block:: php
           :linenos:

            'firewalls' => array(
                'login_firewall' => array(
                    'pattern'   => '^/login$',
                    'anonymous' => array(),
                ),
                'secured_area' => array(
                    'pattern'    => '^/',
                    'form_login' => array(),
                ),
            ),

    **3. Upewnij się, że ``/login_check`` znajduje się poza zaporą**

    Następnie, trzeba się upewnić, że adres URL ``check_path`` (np. ``/login_check``)
    znajduje się poza zaporą, której używa się dla logowania formularzowego (w tym
    przykładzie, pojedyncza zapora dopasowuje wszystkie adresy URL, łącznie z
    ``/login_check``). Jeśli ``/login_check`` nie zostanie dopasowany przez
    jakąkolwiek zaporę, to zgłoszony zostanie wyjątek
    ``Unable to find the controller for path "/login_check"``.

    **4. Nie udostępniaj kontekstu zabezpieczeń przy stosowaniu wielu zapór**

    Jeżeli używa się wiele zapór a uwierzytelnianie realizowane jest na jednej
    z nich, to pozostałe zapory nie będą automatycznie uwierzytelniane.
    Różne zapory są jak odrębne systemy zabezpieczeń. Aby zrealizowaną uwierzytelniania
    na wielu zaporach trzeba jawnie określić odrębne
    ref:`konteksty bezpieczeństwa<reference-security-firewall-context>`
    dla każdej zapory. Dla większości zastosowań wystarczy tylko jedna główna zapora.
    
     **5. Trasowanie stron błędów nie jest objęte przez zapory**

    Ponieważ trasowanie realizowane jest *przed* procesem uwierzytelniania i autoryzacji,
    trasowanie stron błędów nie jest objęte jakakolwiek zaporą. Oznacza to, że nie
    można sprawdzić bezpieczeństwo a nawet dostęp do obiektu użytkownika na tych
    stronach. Więcej informacji na :doc:`/cookbook/controller/error_pages`. 

    
.. index::
   single: bezpieczeństwo; autoryzacja
   single: autoryzacja
    
Autoryzacja
-----------

Pierwszym krokiem w zabezpieczeniu aplikacji jest zawsze uwierzytelnianie – czyli
proces weryfikacji użytkownika. W Symfony uwierzytelnianie można zrealizować na
kilka sposobów: poprzez logowanie formularzowe, podstawowe uwierzytelnianie HTTP
lub nawet poprzez Facebook.

Po uwierzytelnieniu użytkownika rozpoczyna się proces autoryzacji. Autoryzacja
dostarcza standardowy i efektywny sposób dla decydowania o tym, czy użytkownik
może mieć dostęp do jakiegoś zasobu (adresu, obiektu modelu, wywołania metody, ...).
Polega to na przypisaniu określonych ról każdemu użytkownikowi, a następnie
różnych ról dla różnych zasobów.

Są dwa różne aspekty procesu autoryzacji:

#. Użytkownik posiada określony zestaw ról;
#. Dla dostępu do zasobu wymaga się określonej roli.

W tym rozdziale skupimy się nad tym, jak zabezpieczyć różne zasoby
(tj. adresy URL, wywołania metod itd.) przez różne role. Później dowiesz się
więcej o tym, jak tworzone są role i jak są przypisywane użytkownikom.

Zabezpieczenie określonych wzorców URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Najprostszym sposobem zabezpieczenia części aplikacji jest użycie zabezpieczenia
całego wzorca URL. Widzieliśmy to już w pierwszym rozdziale, gdzie wszystko co
pasowało do wzorca ``^/admin`` wymagało roli ``ROLE_ADMIN``.

.. caution::

    Zrozumienie, jak dokładnie działa ``access_control`` jest **bardzo ważne**
    dla świadomości tego, czy aplikacja jest właściwie zabezpieczona. Przeczytaj
    rozdział :ref:`security-book-access-control-explanation`, w celu poznania
    szczegółów.

Można określić wiele wzorców potrzebnych adresów URL – każdy jest wyrażeniem regularnym.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...
            access_control:
                - { path: ^/admin/users, roles: ROLE_SUPER_ADMIN }
                - { path: ^/admin, roles: ROLE_ADMIN }

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <!-- ... -->
            <rule path="^/admin/users" role="ROLE_SUPER_ADMIN" />
            <rule path="^/admin" role="ROLE_ADMIN" />
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            ...,
            'access_control' => array(
                array('path' => '^/admin/users', 'role' => 'ROLE_SUPER_ADMIN'),
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
        ));

.. tip::

    Poprzedzenie ścieżki znakiem ``^`` we wzorcu zapewnia, że zostaną dopasowane
    tylko adresy URL rozpoczynające się od wskazanego po tym znaku fragment ścieżki.
    Na przykład, ścieżka ``/admin`` (bez znaku ``^``) pasuje do adresu ``/admin/foo``,
    ale i też do ``/foo/admin``.


.. index::
   single: bezpieczeństwo; kontrola dostępu

.. _security-book-access-control-explanation:

Wyjaśnienie jak działa ``access_control``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dla przychodzącego żądania Symfony2 sprawdza każdy zapis ``access_control`` aby
znaleźć jeden pasujący do bieżącego żądania. Jak tylko taki wpis zostanie znaleziony,
to wyszukiwanie zostaje zakończone - oznacza to, że wzięty będzie pod uwagę tylko
pierwszy dopasowany wpis ``access_control``.

Każdy ``access_control`` posiada kilka opcji, które konfigurują dwie różne rzeczy:
#. :ref:`dopasowują przychodzącego żądania do zapisu listy kontroli
dostępu <security-book-access-control-matching-options>`
i
#. :ref:`nakładają jakieś ograniczenia, które powinny zostać wyegzekwowane na
dopasowanym adresie URL<security-book-access-control-enforcement-options>`:

.. _security-book-access-control-matching-options:

1. Opcje dopasowujące
.....................

Symfony2 tworzy instancję :class:`Symfony\\Component\\HttpFoundation\\RequestMatcher`
dla każdego wpisu access_control, który określa czy dana reguła kontroli dostępu
powinna być użyta dla danego żądania. Przy dopasowaniu stosowane są następujące opcje
``access_control``:

* ``path``
* ``ip`` lub ``ips``
* ``host``
* ``methods``

Dla przykładu zastosujmy następujące ``wpisy access_control``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...
            access_control:
                - { path: ^/admin, roles: ROLE_USER_IP, ip: 127.0.0.1 }
                - { path: ^/admin, roles: ROLE_USER_HOST, host: symfony.com }
                - { path: ^/admin, roles: ROLE_USER_METHOD, methods: [POST, PUT] }
                - { path: ^/admin, roles: ROLE_USER }

    .. code-block:: xml
       :linenos:

            <access-control>
                <rule path="^/admin" role="ROLE_USER_IP" ip="127.0.0.1" />
                <rule path="^/admin" role="ROLE_USER_HOST" host="symfony.com" />
                <rule path="^/admin" role="ROLE_USER_METHOD" method="POST, PUT" />
                <rule path="^/admin" role="ROLE_USER" />
            </access-control>

    .. code-block:: php
       :linenos:

            'access_control' => array(
                array('path' => '^/admin', 'role' => 'ROLE_USER_IP', 'ip' => '127.0.0.1'),
                array('path' => '^/admin', 'role' => 'ROLE_USER_HOST', 'host' => 'symfony.com'),
                array('path' => '^/admin', 'role' => 'ROLE_USER_METHOD', 'method' => 'POST, PUT'),
                array('path' => '^/admin', 'role' => 'ROLE_USER'),
            ),

Symfony decyduje która reguła ``access_control`` zostanie użyta dla każdego
przychodzącego żądania w oparciu o adres URI, adres IP klienta, nadesłane nazwy
hosta i metody żądania. Trzeba pamiętać, że użyta zostaje pierwsza dopasowana
reguła i jeśli warości ``ip``, ``host`` lub ``method`` nie są określone we wpisie,
to ``access_control`` będzie dopasować każdy ``ip``, ``host`` lub ``method``:

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
..........................

Po tym jak Symfony2 określi, który wpis ``access_control`` zostanie użyty
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

W pewnych sytuacjach może zachodzić potrzeba ograniczenia dostępu dla określonych
adresów IP. Jest to szczególnie istotne w przypadku na przykład
:ref:`Edge Side Includes<edge-side-includes>`. Gdy dostępne jest ESI, to zaleca
się zabezpieczyć dostęp do adresów URL ESI. W rzeczywistości niektóre ESI mogą
zawierać pewne prywatne treści, jak informacje o obecnie zalogowanym użytkowniku.
Aby uniemożliwić dostęp do tych zasobów z poziomu przeglądarki (poprzez odgadywanie
wzorców adresów URL ESI), trasa ESI musi zabezpieczony tak, aby adres taki był
widoczny tylko z bufora zaufanego odwrotnego proxy.

.. versionadded:: 2.3
    Wersja 2.3 umożliwia określenie wielu adresów IP w jednej regule, poprzez
    wykorzystanie konstrukcji ``ips: [a, b]``. W wersjach wcześniejszych trzeba
    było tworzyć oddzielna regułę dla każdego adresu IP z użyciem klucza ``ip``
    a nie ``ips``.

.. caution::

    Jak można przeczytać w wyjaśnieniu poniższego przykładu, opcja ``ip`` nie ogranicza
    się do określonego adresu IP. Zamiast tego, zastosowanie klucza ``ip`` oznacza,
    że zapis ``access_control`` będzie dopasowywany tylko do tego adresu IP,
    a sprawdzenie dostępu użytkowników do tego adresu z innego adresu IP będzie
    dalej kontynuowane w dół listy ``access_control``.


Oto przykład, jak można zabezpieczyć przed dostępem z zewnątrz wszystkie trasy
ESI rozpoczynające się przedrostkiem ``/esi``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...
            access_control:
                - { path: ^/esi, roles: IS_AUTHENTICATED_ANONYMOUSLY, ips: [127.0.0.1, ::1] }
                - { path: ^/esi, roles: ROLE_NO_ACCESS }

    .. code-block:: xml
       :linenos:
       
            <access-control>
                <rule path="^/esi" role="IS_AUTHENTICATED_ANONYMOUSLY"
                    ips="127.0.0.1, ::1" />
                <rule path="^/esi" role="ROLE_NO_ACCESS" />
            </access-control>

    .. code-block:: php
       :linenos:

            'access_control' => array(
                array(
                    'path' => '^/esi',
                    'role' => 'IS_AUTHENTICATED_ANONYMOUSLY',
                    'ips' => '127.0.0.1, ::1'
                ),
                array(
                    'path' => '^/esi',
                    'role' => 'ROLE_NO_ACCESS'
                ),
            ),

Oto jak to działa dla adresu ``/esi/something`` przychodzącego z adresu IP ``10.0.0.1``:

* Pierwsza reguła kontroli dostępu zostaje zignorowana, jako że ``path`` wprawdzie
  pasuje, ale nie zgadza się z jednym z wymienionych adresów ``ip``;

* Druga reguła kontroli dostępu zostaje włączona (jedynym ograniczeniem jest ``path``,
  które pasuje) - jako że użytkownik nie może mieć roli ``ROLE_NO_ACCESS``, której
  nie określono, dostęp jest zabroniony (rola ``ROLE_NO_ACCESS`` może być czymś,
  co nie pasuje do istniejącej roli, to po prostu tylko trik, zawsze uniemożliwiający
  dostęp).

Teraz, gdy to samo żądanie przyjdzie z serwera ``127.0.0.1`` lub ``::1`` (adres
pętli zwrotnej IPv6):

* Teraz pierwsza reguła kontroli dostępu zostaje włączona, gdyż zarówno ``path``
  jak i ``ip`` zostają dopasowane – dostęp jest dozwolony jako że użytkownik zawsze
  ma rolę ``IS_AUTHENTICATED_ANONYMOUSLY``;

* Druga reguła kontroli dostępu nie jest sprawdzana, bo dopasowana została już
  pierwsza reguła.


.. index::
   single: bezpieczeństwo; zabezpieczenie przez kanał

.. _book-security-securing-channel:
 
   
Wymuszanie kanału
~~~~~~~~~~~~~~~~~

Można również zażądać aby użytkownik otrzymał dostęp do adresu URL poprzez SSL.
Wystarczy użyć argument ``requires_channel`` we wpisie ``access_control``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...
            access_control:
                - { path: ^/cart/checkout, roles: IS_AUTHENTICATED_ANONYMOUSLY, requires_channel: https }

    .. code-block:: xml
       :linenos:

            <access-control>
                <rule path="^/cart/checkout" role="IS_AUTHENTICATED_ANONYMOUSLY" requires_channel="https" />
            </access-control>

    .. code-block:: php
       :linenos:

            'access_control' => array(
                array('path' => '^/cart/checkout', 'role' => 'IS_AUTHENTICATED_ANONYMOUSLY', 'requires_channel' => 'https'),
            ),


.. index::
   single: bezpieczeństwo; użytkownicy
   single: bezpieczeństwo; dostawcy użytkowników
   single: uwierzytelnianie; użytkownicy

Użytkownicy
-----------

W poprzednich rozdziałach dowiedziałeś się jak można chronić różne zasoby
przydzielając zestaw ról do zasobu. W tej sekcji wyjaśnimy inną stronę autoryzacji - 
użytkowników.

Skąd się biorą użytkownicy? (*Dostawcy użytkowników*)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Podczas uwierzytelniania użytkownik przesyła zestaw poświadczeń (zwykle nazwę
użytkownika i hasło). Zadaniem systemu uwierzytelniania jest dopasowanie tych
poświadczeń do jakiejś puli użytkowników. Skąd więc pochodzi ta lista użytkowników?

W Symfony2 użytkownicy mogą pochodzić z dowolnego źródła: pliku konfiguracyjnego,
tabeli bazy danych, serwisu internetowego i innych miejsc. Wszystko, co dostarcza
jednego lub więcej użytkowników do systemu uwierzytelniania jest nazywane
„dostawcą użytkowników" (*ang. User Provider*). Symfony2 ma wbudowanych standardowo
dwóch popularnych dostawców użytkowników: pierwszy ładuje użytkowników z pliku
konfiguracyjnego, a drugi z tabeli bazy danych.

Określanie użytkowników w pliku konfiguracyjnym
...............................................

Najprostszym sposobem określenia użytkowników jest bezpośrednie ich określenie
w pliku konfiguracyjnym. Pokażemy to na przykładzie w tym rozdziale.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...
            providers:
                default_provider:
                    memory:
                        users:
                            ryan:  { password: ryanpass, roles: 'ROLE_USER' }
                            admin: { password: kitten, roles: 'ROLE_ADMIN' }

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <!-- ... -->
            <provider name="default_provider">
                <memory>
                    <user name="ryan" password="ryanpass" roles="ROLE_USER" />
                    <user name="admin" password="kitten" roles="ROLE_ADMIN" />
                </memory>
            </provider>
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            ...,
            'providers' => array(
                'default_provider' => array(
                    'memory' => array(
                        'users' => array(
                            'ryan' => array('password' => 'ryanpass', 'roles' => 'ROLE_USER'),
                            'admin' => array('password' => 'kitten', 'roles' => 'ROLE_ADMIN'),
                        ),
                    ),
                ),
            ),
        ));

Taki dostawca użytkowników jest nazywany dostawcą "z pamięci"
(*ang. In-memory Provider*), ponieważ użytkownicy nie są przechowywani
w jakiejkolwiek bazie danych. Faktyczny obiekt użytkownika dostarczany jest tu
przez Symfony (:class:`Symfony\\Component\\Security\\Core\\User\\User`).

.. tip::

    Każdy dostawca użytkownika może załadować użytkowników bezpośrednio
    z konfiguracji przez określenie parametru konfiguracji użytkowników
    i wyszczególnienie w nim użytkowników.

.. caution::

    Jeżeli nazwa użytkownika jest w całości numeryczna (np. ``77``) lub zawiera
    myślnik (np. ``user-name``), to podczas określania użytkowników w YAML trzeba
    użyć alternatywnej składni:

    .. code-block:: yaml
       :linenos:

        users:
            - { name: 77, password: pass, roles: 'ROLE_USER' }
            - { name: user-name, password: pass, roles: 'ROLE_USER' }

W mniejszych witrynach metoda ta jest szybka i łatwa w ustawieniu. Dla bardziej
złożonych systemów, trzeba już ładować użytkowników z bazy danych.


.. _book-security-user-entity:

Ładowanie użytkowników z bazy danych
....................................

Jeżeli chce się ładować użytkowników za pomocą Doctrine ORM, można to łatwo zrobić
przez utworzenie klasy *User* i skonfigurowanie dostawcy encji
(*ang. Entity Provider*).

.. tip::

    Dostępny jest wysokiej jakości pakiet otwartego źródła, który umożliwia
    przechowywanie użytkowników poprzez Doctrine ORM lub ODM. Więcej informacji
    na `FOSUserBundle`_ na GitHub.

Przy takim podejściu, należy najpierw stworzyć własną klasę User, która będzie
przechowywana w bazie danych.

.. code-block:: php
   :linenos:

    // src/Acme/UserBundle/Entity/User.php
    namespace Acme\UserBundle\Entity;

    use Symfony\Component\Security\Core\User\UserInterface;
    use Doctrine\ORM\Mapping as ORM;

    /**
     * @ORM\Entity
     */
    class User implements UserInterface
    {
        /**
         * @ORM\Column(type="string", length=255)
         */
        protected $username;

        // ...
    }

O ile chodzi o system bezpieczeństwa, to istnieje tylko wymóg stworzenia własnej
klasy użytkownika implementującej interfejs :class:`Symfony\\Component\\Security\\Core\\User\\UserInterface`.
Oznacza to, że pojęcie "użytkownika" jest wystarczające, tak długo, jak długo implementuje ten interfejs.

.. note::

    Obiekt użytkownika zostanie serializowany i zapisany w sesji podczas przetwarzania
    żądania, dlatego zaleca się , aby implementować `interfejs Serializable`_
    w swoim obiekcie. Jest to szczególnie ważne, gdy klasa ma klasę nadrzędną
    z prywatnymi własnościami.

Następnie należy skonfigurowac dostawcę encji użytkowników i wskazać to swojej
klasie User:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            providers:
                main:
                    entity: { class: Acme\UserBundle\Entity\User, property: username }

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <provider name="main">
                <entity class="Acme\UserBundle\Entity\User" property="username" />
            </provider>
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'providers' => array(
                'main' => array(
                    'entity' => array('class' => 'Acme\UserBundle\Entity\User', 'property' => 'username'),
                ),
            ),
        ));

Wraz z wprowadzeniem tego nowego dostawcy system uwierzytelniania spróbuje załadować
obiekt ``User`` z bazy danych, wykorzystując pole ``username`` tej klasy.

.. note::

    :doc:`Jak załadować użytkowników systemu bezpieczeństwa z bazy danych
    (dostawca encji)</cookbook/security/entity_provider>`.

Więcej informacji o tworzeniu własnego dostawy (np. jeśli potrzeba ładować
użytkowników poprzez serwis internetowy), znajdziesz w artykule
:doc:`Jak utworzyć własnego dostawcę użytkowników</cookbook/security/custom_provider>`.

.. index::
   single: bezpieczeństwo; kodowanie hasła

.. _book-security-encoding-user-password:

Kodowanie hasła użytkowników
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Do tej pory, dla uproszczenia, we weszystkich przykładach hasło użytkownika było
przechowywane jako zwykły tekst (nawet dla tych użytkowników przechowywanych
w pliku konfiguracyjnym czy bazie danych). Oczywiście w prawdziwej aplikacji
będziemy chcieli kodować hasła użytkowników w celach bezpieczeństwa.
Można to łatwo wykonać przez odwzorowanie klasy *User* na jeden z kilku
wbudowanych "koderów". Na przykład, aby przechowywać użytkowników w pamięci,
ale zasłonić ich hasła za pomocą sha1, zrób to co poniżej:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...
            providers:
                in_memory:
                    memory:
                        users:
                            ryan:
                                password: $2a$12$w/aHvnC/XNeDVrrl65b3dept8QcKqpADxUlbraVXXsC03Jam5hvoO
                                roles: 'ROLE_USER'
                            admin:
                                password: $2a$12$HmOsqRDJK0HuMDQ5Fb2.AOLMQHyNHGD0seyjU3lEVusjT72QQEIpW
                                roles: 'ROLE_ADMIN'

            encoders:
                Symfony\Component\Security\Core\User\User:
                    algorithm: bcrypt
                    cost: 12

    .. code-block:: xml
       :linenos:

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
                        <user
                            name="ryan"
                            password="$2a$12$w/aHvnC/XNeDVrrl65b3dept8QcKqpADxUlbraVXXsC03Jam5hvoO"
                            roles="ROLE_USER" />
                        <user
                            name="admin"
                            password="$2a$12$HmOsqRDJK0HuMDQ5Fb2.AOLMQHyNHGD0seyjU3lEVusjT72QQEIpW"
                            roles="ROLE_ADMIN" />
                    </memory>
                </provider>

                <encoder
                    class="Symfony\Component\Security\Core\User\User"
                    algorithm="bcrypt"
                    cost="12" />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'providers' => array(
                'in_memory' => array(
                    'memory' => array(
                        'users' => array(
                            'ryan' => array(
                                'password' => '$2a$12$w/aHvnC/XNeDVrrl65b3dept8QcKqpADxUlbraVXXsC03Jam5hvoO',
                                'roles' => 'ROLE_USER',
                            ),
                            'admin' => array(
                                'password' => '$2a$12$HmOsqRDJK0HuMDQ5Fb2.AOLMQHyNHGD0seyjU3lEVusjT72QQEIpW',
                                'roles' => 'ROLE_ADMIN',
                            ),
                        ),
                    ),
                ),
            ),
            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => array(
                    'algorithm'         => 'bcrypt',
                    'iterations'        => 12,
                ),
            ),
        ));

.. versionadded:: 2.2
    Koder BCrypt został wprowadzony do Symfony 2.2.


Przez ustawienie opcji ``iterations`` na 1 a ``encode_as_base64`` na false, hasło
jest przepuszczane przez algorytm sha1 tylko raz i bez dodatkowego szyfrowania.
Można obliczyć programowo hasło haszowane (np. hash('sha1', 'ryanpass')) lub poprzez
narzędzia internetowe, takie jak `functions-online.com`_.

Jeśli użytkownicy tworzeni sa dynamicznie (i przechowuje się ich w bazie danych),
to można użyć nawet bardziej złożonych algorytmów haszujących i następnie powoływać
się na rzeczyswisty obiekt kodera haseł aby pomóc w kodowaniu haseł. Na przykład,
przyjmijmy, że obiekt ``User`` to ``Acme\UserBundle\Entity\User`` (podobnie jak w powyższym
przykładzie). Najpierw skonfigurujemy koder dla tego użytkownika:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            encoders:
                Acme\UserBundle\Entity\User: sha512

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <!-- ... -->

            <encoder class="Acme\UserBundle\Entity\User" algorithm="sha512" />
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            ...,
            'encoders' => array(
                'Acme\UserBundle\Entity\User' => 'sha512',
            ),
        ));

W tym przypadku użyliśmy silniejszego algorytmu ``sha512``. Ponadto ponieważ mamy
jasno określony algorytm (``sha512``) jako łańcuch tekstowy, system bedzie domyślnie
mieszał podane hasło 5000 razy z rzędu i następnie zakoduje je jako *base64*.
Innymi słowami, hasło zostało bardzo ukryte, tak więc zakodowane tak hasło nie
może być rozkodowane (tzn. nie można określić hasła z zakodowanego hasła).

.. tip::

    Możliwe jest również stosowanie różnych algorytmów mi mieszających na bazie
    "user-by-user". Więcej informacji w :doc:`/cookbook/security/named_encoders`.

Ustalenie zakodowanego hasła
............................

Jeśli ma się jakiś formularz rejestracyjny dla użytkowników, to zachodzi potrzeba
określenia zakodowanego hasła, tak aby można było ustalić go dla użytkownika.
Bez względu na skonfigurowany algorytm dla obiektu użytkownika, zakodowane hasło
można zawsze określić w następujący sposób w kontrolerze::

    $factory = $this->get('security.encoder_factory');
    $user = new Acme\UserBundle\Entity\User();

    $encoder = $factory->getEncoder($user);
    $password = $encoder->encodePassword('ryanpass', $user->getSalt());
    $user->setPassword($password);

.. index::
   single: bezpieczeństwo; obiekt User
   songle: uwierzytelnianie; obiekt User
   pair: klasa; User

Pobieranie obiektu użytkownika
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Po uwierzytelnieniu obiekt ``User`` bieżącego użytkowanika może być dostępny
poprzez usługę ``security.context``. Od wnętrza kontrolera wygląda to tak::

    public function indexAction()
    {
        $user = $this->get('security.context')->getToken()->getUser();
    }


W kontrolerze może to zostać skrócone do:

.. code-block:: php

    public function indexAction()
    {
        $user = $this->getUser();
    }


.. note::

    Użytkownicy anonimowi są automatycznie uwierzytelniani, co oznacza, że metoda
    ``isAuthenticated()`` obiektu anonimowego użytkownika zwraca ``true``.
    Aby sprawdzić czy użytkownik jest rzeczywiście uwierzytelniony dokonaj sprawdzenia
    dla roli ``IS_AUTHENTICATED_FULLY``.

W szablonie Twig obiekt ten może być dostępny poprzez klucz ``app.user``,
który wywołuje metodę ``GlobalVariables::getUser()``:

.. configuration-block::

    .. code-block:: html+jinja

        <p>Username: {{ app.user.username }}</p>

    .. code-block:: html+php

        <p>Username: <?php echo $app->getUser()->getUsername() ?></p>


Stosowanie wielu dostawców użytkowników
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Każdy mechanizm uwierzytelniania (np. uwierzytelnianie HTTP, logowanie formularzowe,
itd.) używa dokładnie jednego dostawcę użytkowników i zastosuje domyślnie pierwszego
zadeklarowanego dostawcę. Ale co, gdy chce się określić kilku użytkowników poprzez
konfigurację a pozostałych umieścić w bazie danych? Jest to możliwe przez stworzenie
nowego dostawcy, który połączy te dwa źródła:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            providers:
                chain_provider:
                    chain:
                        providers: [in_memory, user_db]
                in_memory:
                    memory:
                        users:
                            foo: { password: test }
                user_db:
                    entity: { class: Acme\UserBundle\Entity\User, property: username }

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <provider name="chain_provider">
                <chain>
                    <provider>in_memory</provider>
                    <provider>user_db</provider>
                </chain>
            </provider>
            <provider name="in_memory">
                <memory>
                    <user name="foo" password="test" />
                </memory>
            </provider>
            <provider name="user_db">
                <entity class="Acme\UserBundle\Entity\User" property="username" />
            </provider>
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'providers' => array(
                'chain_provider' => array(
                    'chain' => array(
                        'providers' => array('in_memory', 'user_db'),
                    ),
                ),
                'in_memory' => array(
                    'memory' => array(
                       'users' => array(
                           'foo' => array('password' => 'test'),
                       ),
                    ),
                ),
                'user_db' => array(
                    'entity' => array('class' => 'Acme\UserBundle\Entity\User', 'property' => 'username'),
                ),
            ),
        ));

Teraz wszystkie mechanizmy uwierzytelniania będą używać ``chain_provider``,
ponieważ jest on określony jako pierwszy. Z kolei ``chain_provider`` będzie próbował
załadować użytkowników z pozostałych źródeł: od dostawców ``in_memory`` i ``user_db``.

Można też skonfigurować zaporę lub poszczególne mechanizmy uwierzytelniania do
stosowania określonego dostawcy. Jeśli dostawca nie jest określony jawnie,
to jak poprzednio, zawsze stosowany będzie pierwszy dostawca:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    # ...
                    provider: user_db
                    http_basic:
                        realm: "Secured Demo Area"
                        provider: in_memory
                    form_login: ~

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <firewall name="secured_area" pattern="^/" provider="user_db">
                <!-- ... -->
                <http-basic realm="Secured Demo Area" provider="in_memory" />
                <form-login />
            </firewall>
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    ...,
                    'provider' => 'user_db',
                    'http_basic' => array(
                        ...,
                        'provider' => 'in_memory',
                    ),
                    'form_login' => array(),
                ),
            ),
        ));

W tym przykładzie, jeśli użytkownik spróbuje się zalogować poprzez uwierzytelnianie
HTTP, to system uwierzytelniania będzie używał użytkowników ``in_memory``. Lecz gdy
użytkownik spróbuje zalogować się poprzez logowanie formularzowe, to wybrany będzie
dostawca ``user_db`` (ponieważ taki dostawca jest dostawcą domyślnym dla zapory w ogóle).

Więcej inforamcji o dostawcach użytkowników i konfiguracji zapory znajduje się
w artykule :doc:`/reference/configuration/security`.








.. index::
   single: bezpieczeństwo; role
   single: role
   single: autoryzacja; role

.. _book-security-role-hierarchy:

Role
----

Pojęcie "roli" jest kluczem do procesu autoryzacji. Każdemu użytkownikowi jest
przypisany zestaw ról i z kolei każdy zasób wymaga jednej lub więcej ról aby
mieć do niego dostęp. Jeżeli użytkownik ma wymagane role, to dostęp jest udzielony.
W przeciwnym razie dostęp jest zabroniony.

Role są bardzo proste - są to łańcuchy tekstowe, które można
sobie wymyślić i używać w razie potrzeby (jakby były obiektami wewnętrznymi).
Na przykład, jeśli chce się wprowadzić ograniczenia dostępu do części administracyjnej
sekcji blogu na witrynie, to można chronić tą sekcję stosując rolę
``ROLE_BLOG_ADMIN``. Nie trzeba definiować tej roli nigdzie – wystarczy
rozpocząć ją używać.

.. note::

    Wszystkie role, by mogły być zarządzane prze Symfony2, **muszą** rozpoczynać
    się przedrostkiem ``ROLE_``. Jeśli zdefiniuje się własne role stosując
    dedykowana klasę ``Role`` (bardziej zaawansowane), nie trzeba wówczas stosować
    przedrostka ``ROLE_``.

.. _book-security-role-hierarchy:

Role hierarchiczne
~~~~~~~~~~~~~~~~~~

Zamiast przypisywać użytkownikowi wiele ról, można zdefiniować dziedziczenie ról,
tworząc ich hierarchię:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            role_hierarchy:
                ROLE_ADMIN:       ROLE_USER
                ROLE_SUPER_ADMIN: [ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH]

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <role id="ROLE_ADMIN">ROLE_USER</role>
            <role id="ROLE_SUPER_ADMIN">ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH</role>
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'role_hierarchy' => array(
                'ROLE_ADMIN'       => 'ROLE_USER',
                'ROLE_SUPER_ADMIN' => array('ROLE_ADMIN', 'ROLE_ALLOWED_TO_SWITCH'),
            ),
        ));

W powyższej konfiguracji użytkownicy z rolą ``ROLE_ADMIN`` będą również mieć rolę
``ROLE_USER``. Rola ``ROLE_SUPER_ADMIN`` ma role ``ROLE_ADMIN``,
``ROLE_ALLOWED_TO_SWITCH`` i ``ROLE_USER`` (odziedziczoną z ``ROLE_ADMIN``).

.. index::
   single: bezpieczeństwo; kontrola dostępu
   single: kontrola dostępu


Kontrola dostępu
----------------

Teraz, gdy ma się użytkownika i role, można pójść dalej, niż autoryzacja w oparciu
o wzorzec URL.

.. _book-security-securing-controller:

Kontrola dostępu w kontrolerze
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ochrona aplikacji na podstawie wzorców URL jest łatwa, ale w niektórych przypadkach
może się okazać nie wystarczającą. Gdy zajdzie konieczność, można łatwo wymusić
autoryzację wewnątrz kontrolera::

    // ...
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;

    public function helloAction($name)
    {
        if (false === $this->get('security.context')->isGranted('ROLE_ADMIN')) {
            throw new AccessDeniedException();
        }

        // ...
    }

.. caution::

   Zapora musi być aktywna, w przeciwnym razie, podczas wywoływania metody
   ``isGranted()`` zostanie zrzucony wyjątek.. Zawsze dobrym pomysłem jest,
   posiadanie głównej zapory obejmującej wszystkie ścieżki URL (tak jak
   pokazano to w tym rozdziale).

Można także wybrać do zainstalowania i stosowania opcjonalny pakiet `JMSSecurityExtraBundle`_,
który może zabezpieczać kontroler przy użyciu adnotacji::

    // ...
    use JMS\SecurityExtraBundle\Annotation\Secure;

    /**
     * @Secure(roles="ROLE_ADMIN")
     */
    public function helloAction($name)
    {
        // ...
    }

Kontrola dostępu w innych usługach
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W istocie w Symfony można zabezpieczyć wszystko stosując strategie podobną do omówionej
w poprzednim rozdziale. Na przykład załóżmy, że mamy jakąś usługę (np. klasę PHP),
której zadaniem jest wysłanie wiadomości email od jednego użytkownika do drugiego.
Można ograniczyć korzystanie z tej klasy – bez względu gdzie jest ona używana, użyć
ją może tylko użytkownik posiadający określoną role.

Więcej informacji o tym jak można wykorzystać komponent Security do zabezpieczenia
różnych usług i metod w swojej aplikacji znajduje się w
:doc:`/cookbook/security/securing_services`.

.. _book-security-template:

Kontrola dostępu w szablonach
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli chce się sprawdzić, czy bieżący użytkownik posiada jakąś rolę wewnątrz
szablonu, należy użyć wbudowaną w Twig funkcję pomocniczą:

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        {% if is_granted('ROLE_ADMIN') %}
            <a href="...">Delete</a>
        {% endif %}

    .. code-block:: html+php
       :linenos:

        <?php if ($view['security']->isGranted('ROLE_ADMIN')): ?>
            <a href="...">Delete</a>
        <?php endif; ?>

.. note::

    W przypadku użycia tej funkcji gdy na zaporze *nie ma* określonych ścieżek URL,
    zostanie zrzucony wyjątek. Więc jeszcze raz, zawsze dobrym pomysłem jest posiadanie
    głównej zapory, która obejmuje wszystkie ścieżki URL (tak jak pokazano to w
    tym rozdziale).

.. _book-security-template-expression:

.. versionadded:: 2.4
    Funkcjonalność *wyrażeń dostępowych* została wprowadzona w Symfony 2.4.

Mozna również uzyć wyrażeń dostępowych w szablonach:

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        {% if is_granted(expression(
            '"ROLE_ADMIN" in roles or (user and user.isSuperAdmin())'
        )) %}
            <a href="...">Delete</a>
        {% endif %}

    .. code-block:: html+php
       :linenos:

        <?php if ($view['security']->isGranted(new Expression(
            '"ROLE_ADMIN" in roles or (user and user.isSuperAdmin())'
        ))): ?>
            <a href="...">Delete</a>
        <?php endif; ?>

Więcej informacji o wyrazeniach dostępowych znajduje się w :ref:`book-security-expressions`.

Listy kontroli dostępu (ACL) - zabezpieczanie poszczególnych obiektów w bazie danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Proszę sobie wyobrazić, że zaprojektowaliśmy system bloga, gdzie użytkownicy mogą
komentować wpisy. Teraz chcemy, aby użytkownik mógł edytować własne komentarze,
ale nie komentarze innych usług. Ponadto chcemy, aby administrator mógł edytować
wszystkie blogi.

Komponent Security dostarczany jest z opcjonalny systemem list kontroli dostępu
(ang. Access Control Lists - ACL), które można wykorzystać, gdy zachodzi potrzeba
kontroli dostępu do poszczególnych instancji obiektu w swoim systemie.
Bez ACL można zabezpieczyć system tak, aby tylko niektórzy użytkownicy mogli edytować
wszystkie komentarze. Natomiast z ACL, można ograniczyć lub uniemożliwić dostęp
do określonych komentarzy.

Więcej informacji można znaleźć w artykule :doc:`/cookbook/security/acl`.

.. index::
   single: bezpieczeństwo; wylogowanie
   single: wylogowanie

.. _book-security-logging-out:

Wylogowanie
-----------

Zazwyczaj chce się aby użytkownik aplikacji miał możliwość wylogowania się.
Na szczęście zapora może to obsługiwać automatycznie, jeżeli aktywuje się
parametr konfiguracyjny ``logout``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:  

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    # ...
                    logout:
                        path:   /logout
                        target: /
            # ...

    .. code-block:: xml
       :linenos:
         
        <!-- app/config/security.xml -->
        <config>
            <firewall name="secured_area" pattern="^/">
                <!-- ... -->
                <logout path="/logout" target="/" />
            </firewall>
            <!-- ... -->
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    // ...
                    'logout' => array('path' => 'logout', 'target' => '/'),
                ),
            ),
            // ...
        ));

Po skonfigurowaniu tego pod zaporą, gdy użytkownik wyśle ``/logout`` (lub coś
innego, co zostało podane w ``path``), spowoduje się od uwierzytelnienie bieżącego
użytkownika. Użytkownik zostanie przekierowany do strony głównej (wartości określonej
w parametrze ``target``). Wartości domyślne parametru konfiguracyjnego ``path``
jak i ``target`` to wartości tam podane. Innymi słowami, o ile nie chcesz ich zmieniać,
to możesz je pominąć i skrócic swoja konfigurację:

.. configuration-block::

    .. code-block:: yaml

        logout: ~

    .. code-block:: xml

        <logout />

    .. code-block:: php

        'logout' => array(),

Należy pamiętać, że nie potrzeba implementować kontrolera dla trasy adresu URL
``/logout`` gdyż o wszystko troszczy się zapora. Zdawać sobie trzeba jednak sprawę,
że należy utworzyć trasę, tak aby można było używać jej do generowania adresu URL:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        logout:
            path:   /logout

    .. code-block:: xml
       :linenos:
       
        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="logout" path="/logout" />
        </routes>

    ..  code-block:: php
        :linenos:
        
        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('logout', new Route('/logout', array()));

        return $collection;

Gdy użytkownik zostanie wylogowany, to nastąpi przekierowany do miejsca określonego
w parametrze ``target`` (np. do ``homepage``). Więcej informacji o konfiguracji
wylogowania znajdziesz w artykule
:doc:`Informacja o konfiguracji bezpieczeństwa</reference/configuration/security>`.

.. index::
   single: bezpieczeństwo; uwierzytelnianie bezstanowe
   single: uwierzytelnianie; bezstanowe

Uwierzytelnianie bezstanowe
---------------------------

Domyślnie Symfony2 opiera się na pliku cookie (Sesja) w celu utrzymania kontekstu
bezpieczeństwa użytkownika. Lecz jeśli używa się certyfikatów uwierzytelniania HTTP,
utrzymywanie tego kontekstu nie jest potrzebne, bo poświadczenia są dostępne od razu
dla każdego żądania. W tym przypadku, gdy nie trzeba przechowywać czegokolwiek
pomiędzy żądaniami, można aktywować uwierzytelnianie bezstanowe (co oznacza, że
Symfony 2 nie będzie tworzyć pliku cookie):

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                main:
                    http_basic: ~
                    stateless:  true

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <config>
            <firewall stateless="true">
                <http-basic />
            </firewall>
        </config>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('http_basic' => array(), 'stateless' => true),
            ),
        ));

.. note::

    Jeśli używa się logowania formularzowego, Symfony2 będzie tworzył plik cookie
    nawet wówczas, gdy ustawi się ``stateless`` na ``true``.

.. index::
   pair: bezpieczeństwo; narzędzia

Narzędzia
---------

Komponent bezpieczeństwa Symfony dostarczany jest z kolekcją przyjemnych narzędzi
związanych z bezpieczeństwem. Narzędzia te używane są przez Symfony, ale można je
również stosować , jeśli chce się rozwiązać swoje problemy bezpieczeństwa przy
ich użyciu.

.. index::
   single: bezpieczeństwo, ataki czasowe

Porównanie łańcuchów tekstowych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Czas potrzebny do porównania dwóch ciągów znakowych zależy od różnic w tych ciągach.
Jest to podstawa tzw. `ataków czasowych`_ (*ang. timing attack*), które stały się
ostatnio realnym zagrożeniem w internecie.

Wewnętrznie, gdy porównuje się dwa hasła, Symfony używa algorytmu o stałym czasie
wykonania. Można użyć tej samej strategii we własnym kodzie, dzięki klasie
:class:`Symfony\\Component\\Security\\Core\\Util\\StringUtils`::

    use Symfony\Component\Security\Core\Util\StringUtils;

    // is password1 equals to password2?
    $bool = StringUtils::equals($password1, $password2);

Generowanie bezpiecznej liczby losowej
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Gdy trzeba wygenerować bezpieczną liczbę losową, mocno zachęcany jesteś do
korzystania z klasy Symfony
:class:`Symfony\\Component\\Security\\Core\\Util\\SecureRandom`::

    use Symfony\Component\Security\Core\Util\SecureRandom;

    $generator = new SecureRandom();
    $random = $generator->nextBytes(10);

Metoda
:method:`Symfony\\Component\\Security\\Core\\Util\\SecureRandom::nextBytes`
zwraca losowy ciąg składający się ze znaków w ilości określonej w argumencie
(10 w powyższym przykładzie).

Klasa SecureRandom działa lepiej z instalacją OpenSSL, ale gdy to nie jest możliwe,
to wraca do wewnętrznego algorytmu, który wymaga do prawidłowego działania dostępu
do pliku *seed*. Wystarczy podać nazwę pliku aby to umożliwić::

    $generator = new SecureRandom('/some/path/to/store/the/seed.txt');
    $random = $generator->nextBytes(10);

.. note::

    Można również uzyskać dostęp do bezpiecznej losowej instancji bezpośrednio
    z kontenera wstrzykiwania zalezności Symfony, Jego nazwa, to
    ``security.secure_random``.
    

Wnioski końcowe
---------------

Ochrona dostępu w aplikacji powinna być głęboka i rozwiązywać poprawnie
skomplikowane problemy bezpieczeństwa. Na szczęście, komponent bezpieczeństwa Symfony
spełnia bardzo dobrze te zadania opierając się na modelu  *uwierzytelniania*
i *autoryzacji*. Uwierzytelnianie, które zawsze realizowane jest w pierwszej kolejności,
jest obsługiwane przez zaporę, której zadaniem jest ustalenie tożsamości użytkownika,
przy wykorzystaniu różnych metod (np. uwierzytelnianie HTTP, logowanie formularzowe itd.).
W Receptariuszu znajdziesz przykłady innych metod obsługujących uwierzytelnianie,
w tym implementację  cookie funkcjonalności "remember me".

Po uwierzytelnieniu użytkownika, warstwa autoryzacji może określić czy użytkownik
posiada uprawnienia dostępu do określonego zasobu. Autoryzacja opiera się na
mechaniźmie kontrolnych list dostępowych (ACL) i *rolach*. Najczęściej role są
stosowane do adresów URL, klas lub metod i jeśli bieżący użytkownik nie ma takiej
roli, dostęp jest zabroniony. Warstwa autoryzacji jest jednak głębsza i naśladuje
system "głosowania", tak że wiele elementów aplikacji może określić, czy bieżący
użytkownik może mieć dostęp do danych zasobów. Dowiedz się więcej o tym i innych
tematach w Receptariuszu.

Dalsza lektura
--------------

* :doc:`Wymuszanie HTTP/HTTPS </cookbook/security/force_https>`
* :doc:`Personifikacja użytkownika </cookbook/security/impersonating_user>`
* :doc:`Czarna lista użytkowników poprzez adres Ip z własnym wyborcą </cookbook/security/voters>`
* :doc:`Listy kontroli dostępu (ACL) </cookbook/security/acl>`
* :doc:`/cookbook/security/remember_me`
* :doc:`Jak ograniczyć zapory do określonego hosta </cookbook/security/host_restriction>`

.. _`komponent bezpieczeństwa Symfony`: https://github.com/symfony/Security
.. _`JMSSecurityExtraBundle`: http://jmsyst.com/bundles/JMSSecurityExtraBundle/1.2
.. _`FOSUserBundle`: https://github.com/FriendsOfSymfony/FOSUserBundle
.. _`interfejs Serializable`: http://php.net/manual/en/class.serializable.php
.. _`functions-online.com`: http://www.functions-online.com/sha1.html
.. _`ataków czasowych`: http://en.wikipedia.org/wiki/Timing_attack
