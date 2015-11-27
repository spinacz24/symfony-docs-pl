
Jak zbudować tradycyjny formularz logowania
===========================================

.. tip::

    Jeśli potrzebuje się formularzalogowania i przechowywania użytkowników
    w jakiejś bazie danych, należy rozważyć użycie pakietu `FOSUserBundle`_,
    który pomaga zbudować własny obiekt ``User`` i daje wiele tras oraz
    kontrolerów dla popularnych zadań, takich jak logowanie, rejestrowanie
    i procedura zapomnienia hasła.

W tym artkule będziemy budować tradycyjny formularz logowania. Oczywiście, gdy
użytkownik się loguje, można ładować swoich użytkowników skądkolwiek, w tym z bazy
danych. Szczegóły można znaleźć w rozdziale :ref:`security-user-providers`.

Zakładamy tutaj, że że Czytelnik przeczytał już rozdział   
:doc:`Bezpieczeństwo </book/security>` podręcznika Symfony i ma na swoim komputerze
prawidłowo działajace uwierzytelnianie ``http_basic``.

Najpierw, udostępnijmy logowanie formularzowe w zaporze:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                default:
                    anonymous: ~
                    http_basic: ~
                    form_login:
                        login_path: /login
                        check_path: /login_check

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="default">
                    <anonymous />
                    <http-basic />
                    <form-login login-path="/login" check-path="/login_check" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'default' => array(
                    'anonymous'  => null,
                    'http_basic' => null,
                    'form_login' => array(
                        'login_path' => '/login',
                        'check_path' => '/login_check',
                    ),
                ),
            ),
        ));

.. tip::

    Klucze ``login_path`` i ``check_path`` mogą również nazwami tras (ale nie mogą
    mieć obowiązkowych wieloznaczników - np. ``/login/{foo}`` gdzie ``foo`` nie ma
    domyślnej wartości).

Teraz, gdy system bezpieczeństwa inicjuje proces uwierzytelniania, użytkownik
będzie przekierowywany do formularza logowania  ``/login``.
Implementacja tego formularza, to nasze następne zadanie.
Najpierw utwórzmy w pakiecie nowy kontroler ``SecurityController``::

    // src/AppBundle/Controller/SecurityController.php
    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class SecurityController extends Controller
    {
    }

Następnie, tworzymy dwie trasy, po jednej dla każdej scieżki skonfigurowanej wcześniej
w kluczy ``form_login`` konfiguracji (``/login`` i ``/login_check``):

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Controller/SecurityController.php

        // ...
        use Symfony\Component\HttpFoundation\Request;
        use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

        class SecurityController extends Controller
        {
            /**
             * @Route("/login", name="login_route")
             */
            public function loginAction(Request $request)
            {
            }

            /**
             * @Route("/login_check", name="login_check")
             */
            public function loginCheckAction()
            {
                // ta akcja nie będzie wykonywana,
                // ponieważ trasa jest wykorzystywana przez system bezpieczeństwa
            }
        }

    .. code-block:: yaml

        # app/config/routing.yml
        login_route:
            path:     /login
            defaults: { _controller: AppBundle:Security:login }

        login_check:
            path: /login_check
            # no controller is bound to this route
            # as it's handled by the Security system

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="login_route" path="/login">
                <default key="_controller">AppBundle:Security:login</default>
            </route>

            <route id="login_check" path="/login_check" />
            <!-- no controller is bound to this route
                 as it's handled by the Security system -->
        </routes>

    ..  code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('login_route', new Route('/login', array(
            '_controller' => 'AppBundle:Security:login',
        )));

        $collection->add('login_check', new Route('/login_check'));
        // no controller is bound to this route
        // as it's handled by the Security system

        return $collection;

Następnie dodamy logikę do akcji ``loginAction``, która będzie wyświetlać
formularz logowania::

    // src/AppBundle/Controller/SecurityController.php

    public function loginAction(Request $request)
    {
        $authenticationUtils = $this->get('security.authentication_utils');

        // pobranie błędu logowania, jeśli sie taki pojawił
        $error = $authenticationUtils->getLastAuthenticationError();

        // nazwa użytkownika ostatnio wprowadzona przez aktualnego użytkownika
        $lastUsername = $authenticationUtils->getLastUsername();

        return $this->render(
            'security/login.html.twig',
            array(
                // nazwa użytkownika ostatnio wprowadzona przez aktualnego użytkownika
                'last_username' => $lastUsername,
                'error'         => $error,
            )
        );
    }

.. versionadded:: 2.6
    Usługa ``security.authentication_utils`` i klasa
    :class:`Symfony\\Component\\Security\\Http\\Authentication\\AuthenticationUtils`
    zostały wprowadzone w Symfony 2.6.

W tej akcji nie ma żadnej obsługi błedów. Jak zobaczysz za momoent, podczas zgłaszania
formularza przez użytkownika, system bezpieczeństwa automatycznie obsługuje
zgłaszanie formularza. Jeśli użytkownik złożył nieprawidłową nazwę uzytkownika lub
hasło, kod akcji odczyta błąd procedury logowania z systemu bezpieczeństwa, tak
więc będzie można wyswietlić formularz ponownie.

Innymi słowami, zadaniem programisty jest tylko *wyświenie* formularza logowania
bez obsługiwania ewentualnych błedów, jakie mogą sie pojawić, gdyć system bezpieczeństwa
sam dba o sprawdzenie zgłaszanej nazwy użytkownika i hasła i uwierzytelnia użytkownika.

Na koniec utwórzmy szablon:

.. configuration-block::

    .. code-block:: html+jinja

        {# app/Resources/views/security/login.html.twig #}
        {# ... you will probably extends your base template, like base.html.twig #}

        {% if error %}
            <div>{{ error.messageKey|trans(error.messageData, 'security') }}</div>
        {% endif %}

        <form action="{{ path('login_check') }}" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="{{ last_username }}" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            {#
                Jeśli chcesz kontrolować adres URL, użytkownik zostaje
                przekierowany na 'success' (szczegóły poniżej)
                <input type="hidden" name="_target_path" value="/account" />
            #}

            <button type="submit">login</button>
        </form>

    .. code-block:: html+php

        <!-- src/Acme/SecurityBundle/Resources/views/Security/login.html.php -->
        <?php if ($error): ?>
            <div><?php echo $error->getMessage() ?></div>
        <?php endif ?>

        <form action="<?php echo $view['router']->generate('login_check') ?>" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="<?php echo $last_username ?>" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <!--
                Jeśli chcesz kontrolować adres URL, użytkownik zostaje
                przekierowany na 'success' (szczegóły poniżej)
                <input type="hidden" name="_target_path" value="/account" />
            -->

            <button type="submit">login</button>
        </form>


.. tip::

    Zmienna ``error`` przekazywana do szablonu jest instancją klasy
    :class:`Symfony\\Component\\Security\\Core\\Exception\\AuthenticationException`.
    Może zawierać więcej informacji, lub nawet pufne informacje, o braku uwierzytelnienia,
    więc używaj tego mądrze!

Formularz może wyglądać dowolnie, ale jest kilka wymagań:

* Formularz musi przekazywać żądanie POST do ``/login_check``, ponieważ tak właśnie
  to skonfigurowano w kluczu ``form_login``w ``security.yml``.

* Pole nazwy użytkownika musi mieć nazwę ``_username`` a pole hasła nazwę ``_password``.

.. tip::

    Właściwie, to wszystko może być skonfigurowane w kluczu ``form_login``.
    Proszę przeczytać
    :ref:`reference-security-firewall-form-login` w celu poznania szczegółów.

.. caution::

    Ten formularz logowania nie jest obecnie chroniony przed atakami CSRF.
    Proszę zapoznać się z artykułem
    :doc:`/cookbook/security/csrf_in_login_form`, gdzie omówiono zabezpieczenie
    formularza logowania.

To wszystko! Po zgłoszeniu formularza, system bezpieczeństwa sprawdzi automatycznie
poświadczenia użytkownika i albo uwierzytelni użytkownika albo wyśle go z powrotem
do formularza logowania, gdzie może zostać wyświetlony komunikat o błędzie.

Cała procedura wygląda tak:

#. Użytkownik próbuje uzyskać dostęp do chronionego zasobu;
#. Zapora inicjuje procedutę uwierzytelnienia przez przekierowanie użytkownika
   do formularza logowania (``/login``);
#. Strona ``/login`` renderuje formularz logowania poprzez trasę i kontroler,
   utworzone w naszym przykładzie;
#. Użytkownik zgłasza formularz logowania do ``/login_check``;
#. System bezpieczeństwa przechwytuje żądanie, sprawdza przedłożone dane logowania
   użytkownika (poświadczenia) i jeśli są one prawidłowe, to uwierzytelnia użytkownika,
   a w przeciwnym razie, przekierowuje użytkownika z powrotem do formularza
   logowania.

Przekierowanie po udanym logowaniu
----------------------------------

Jeśli zgłoszone dane logowania sa prawidłowe, użytkownik zostanie przekierowany
do oryginalnej strony, która została zarządana (np. ``/admin/foo``). Gdy użytkownik
początkowo przeszedł do strony logowania, zostanie przekierowany do strony początkowej.
Można to dostosować tak, aby umożliwić, na przykład, przekierowanie użytkownika
na określony adres URL.

Wiecej szczegółów na ten temat oraz o tym, jak w ogóle dostosować proces logowania
formularzowego można znaleźć w artykule :doc:`/cookbook/security/form_login`.

.. _book-security-common-pitfalls:

Unikanie typowych pułapek
-------------------------

Przy ustawianiu formularza logowania trzeba uważać na kilka typowych pułapek.

1. Stwórz prawidłowe trasy
~~~~~~~~~~~~~~~~~~~~~~~~~~

Po pierwsze, upewnij się, że masz prawidłowo zdefiniowane trasy ``/login`` i ``/login_check``
i że odpowiadają one wartościom konfiguracyjnym ``login_path`` i ``check_path``.
Popełnienie tutaj błędu będzie skutkować przekierowaniu na stronę 404, zamiast na
 na stronę logowania lub tym, że zgłoszenie formularza nie będzie działać (po
 prostu będzie się w kółko widziało formularz logowania).

2. Upewnij się, strona logowania nie jest zabezpieczona (pętla przekierowań!)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Trzeba też pamiętać, że strona logowania musi być dostępna dla użytkowników anonimowych.
Na przykład, następująca konfiguracja, w której wymaga sie roli ``ROLE_ADMIN``
dla wszystkich adresów URL (w tym też dla ścieżki URL ``/login``), spowoduje
pętlę przekierowań:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml

        # ...
        access_control:
            - { path: ^/, roles: ROLE_ADMIN }

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
                <rule path="^/" role="ROLE_ADMIN" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ...
        'access_control' => array(
            array('path' => '^/', 'role' => 'ROLE_ADMIN'),
        ),

Dodanie kontroli dostępu dopasowującej ``/login/*`` i nie wymagającej żadnego
uwierzytelniania, rozwiązuje problem:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml

        # ...
        access_control:
            - { path: ^/login, roles: IS_AUTHENTICATED_ANONYMOUSLY }
            - { path: ^/, roles: ROLE_ADMIN }

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
                <rule path="^/login" role="IS_AUTHENTICATED_ANONYMOUSLY" />
                <rule path="^/" role="ROLE_ADMIN" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ...
        'access_control' => array(
            array('path' => '^/login', 'role' => 'IS_AUTHENTICATED_ANONYMOUSLY'),
            array('path' => '^/', 'role' => 'ROLE_ADMIN'),
        ),

Ponadto, jeśli zapora nie zezwala na dostęp anonimowy (nie ma klucza ``anonymous``),
trzeba utworzyć specjalną zaporę umożliwiającą dostęp anonimowy do strony logowania:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml

        # ...
        firewalls:
            # order matters! This must be before the ^/ firewall
            login_firewall:
                pattern:   ^/login$
                anonymous: ~
            secured_area:
                pattern:    ^/
                form_login: ~

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
                <firewall name="login_firewall" pattern="^/login$">
                    <anonymous />
                </firewall>

                <firewall name="secured_area" pattern="^/">
                    <form-login />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ...
        'firewalls' => array(
            'login_firewall' => array(
                'pattern'   => '^/login$',
                'anonymous' => null,
            ),
            'secured_area' => array(
                'pattern'    => '^/',
                'form_login' => null,
            ),
        ),

3. Upewnij się, że /login_check znajduje się za zaporą
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Następnie, upewnij się, że ścieżka ``check_path`` (np. ``/login_check``) znajduje
sie za zaporą, którą używasz dla logowania formularzowego (w naszym przykładzie,
pojedyncza zapora dopasowująca wszystkie ścieżki URL, w tym ``/login_check``).
Jeśli ``/login_check`` nie zostanie dopasowany przez jakąkolwiek zaporę, otrzymasz
wyjątek ``Unable to find the controller for path "/login_check"``.

4. W rozwiązaniu z kilkoma zaporami nie współdzielą one tego samego kontekstu zabezpieczeń
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli używasz wielu zapór, to przy uwierzytelnianiu przez jedną zaporę, nie jest
się automatycznie uwierzyutelnionym przez inne zapory.
Poszczególne zapory są jak odrębne systemy bezpieczeństwa. Więc do zrealizowania
uwierzytelniania na wszystkich zaporach musi sie jawnie określić ten sam
:ref:`reference-security-firewall-context` dla tych różnych zapór. Jednak
zazwyczaj dla większości aplikacji, wystaczy jedna główna zapora.

5. Trasowanie stron błędów nie jest objęte przez zapory
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ponieważ trasowanie jest realizowane przed procedurą bezpieczeństwa (uwierzytelniania
i autoryzacji), strony błedów 404 nie są objete żadną zaporą. Oznacza to że,
nie można sprawdzić zabezpieczenia lub nawet uzyskać dostępu do obiektu użytkownika
na tych stronach. W celu poznania szczegółów proszę zapoznać sie z
artykułem :doc:`/cookbook/controller/error_pages`.

.. _`FOSUserBundle`: https://github.com/FriendsOfSymfony/FOSUserBundle
