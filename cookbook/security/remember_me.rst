.. index::
   single: bezpieczeństwo; "zapamietaj mnie"
   single: zapora; remeber_me
   
Jak dodać funkcjonalność logowania "Zapamietaj mnie"
====================================================

Po uwierzytelnieniu użytkownika, jego poswiadczenie jest przechowywane w sesji.
Oznacza to, że po zakończeniu sesji użytkownik zostanie wylogowany i będzie
musiał następnym razem ponownie dostarczyć swoje dane logowania, aby uzyskać
dostęp do aplikacji. Można pozwolić użytkownikowi wybrać opcję umożliwiajacą
zapamiętanie jego danych logowania przy wykorzystaniu ciasteczka, używając do tego
celu sekcję ``remember_me`` zapory:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                default:
                    # ...
                    remember_me:
                        key:      "%secret%"
                        lifetime: 604800 # 1 week in seconds
                        path:     /
                        # by default, the feature is enabled by checking a
                        # checkbox in the login form (see below), uncomment the
                        # following line to always enable it.
                        #always_remember_me: true

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="utf-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="default">
                    <!-- ... -->

                    <!-- 604800 is 1 week in seconds -->
                    <remember-me
                        key="%secret%"
                        lifetime="604800"
                        path="/" />
                    <!-- by default, the feature is enabled by checking a checkbox
                         in the login form (see below), add always-remember-me="true"
                         to always enable it. -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'default' => array(
                    // ...
                    'remember_me' => array(
                        'key'      => '%secret%',
                        'lifetime' => 604800, // 1 week in seconds
                        'path'     => '/',
                        // by default, the feature is enabled by checking a
                        // checkbox in the login form (see below), uncomment
                        // the following line to always enable it.
                        //'always_remember_me' => true,
                    ),
                ),
            ),
        ));

Sekcja ``remember_me`` zapory posiada następujące opcje:

``key`` (**wymagane**)
    Wartość stosowana do szyfrowania treści ciasteczka. Jest tu powszechnie stosowana
    wartość ``secret``, zdefiniowana w pliku ``app/config/parameters.yml`` file.

``name`` (domyślna wartość: ``REMEMBERME``)
    Nazwa ciasteczka używanego do utrzymywania danych logowania użytkownika.
    Jeśli włączysz możliwość ``remember_me`` w różnych zaporach tej samej aplikacji,
    miej pewność, że została zastosowana inna nazwa dla każdego ciasteczka w poczególnych
    zaporach. Inaczej można mieć poważne problemy z bezpieczeństwem aplikacji.

``lifetime`` (domyślna wartość: ``31536000``)
    Liczba sekund w trakcie których będą utrzymywane w ciasteczku dane logowania.
    Domyślnie użytkownik jest zalogowany przez okres jednego roku.

``path`` (domyślna wartość: ``/``)
    Ścieżka dla której będzie zastosowane ciasteczko związane z tą funkcjonalnością.
    Domyślnie jest to ścieżka wskazująca na całą witrynę, ale może to też być
    określona sekcja witryny
    (np. ``/forum``, ``/admin``).

``domain`` (domyślna wartość: ``null``)
    Domena, na dla której ma być zastosowane ciasteczko związane z tą funkcjonalnością.
    Domyślnie ciasteczka są stosowane dla bieżącej domeny, wskazanej przez zmienną
    ``$_SERVER``.

``secure`` (domyślna wartość: ``false``)
    Jeśli ``true``, to ciasteczko związane z tą funkcjonalnością jest wysyłane
    do użytkownika poprzez bezpieczne połączenie HTTPS.

``httponly`` (domyślna wartość: ``true``)
    Jeśli ``true``, to ciasteczko związane z tą funkcjonalnością jest dostępne
    tylko poprzez protokół HTTP. Oznacza to, że ciasteczko nie będzie dostępne
    przez języki skryptowe, takie jak JavaScript.

``remember_me_parameter`` (domyślna wartość: ``_remember_me``)
    Nazwa pola formularza zaznaczanego, by zdecydować o włączeniu funkcjonalności
    "Zapamiętaj mnie". W dalszej części tego artykułu wyjaśniono jak udostępniać
    tą funkcjonalność warunkowo.

``always_remember_me`` (domyślna wartość: ``false``)
    Jeśli ``true``, to wartość ``remember_me_parameter`` jest ignorowana a funkcjonalność
    "Zapamiętaj mnie" jest zawsze dostępna, bez względu na chęć użytkownika końcowego.

``token_provider`` (domyślna wartość: ``null``)
    Określa identyfikator usługi dostawcy tokenu, jaki ma być używany. Domyślnie,
    tokeny sa przechowywane w cisteczku. Na przyklad, mozna chcieć przechowywać
    token w bazie danych, tak aby nie mieć szyfrowanej wersji hasła w ciasteczku.
    DoctrineBridge zawiera ``Symfony\Bridge\Doctrine\Security\RememberMe\DoctrineTokenProvider``,
    który można wykorzystać tym celu.

Zmuszanie użytkownika do wyboru funkcjonalności "Zapamiętaj mnie"
-----------------------------------------------------------------

Dobrym pomysłem jest udostępnienie użytkownikowi opcji wyboru właczenia funkcjonalności
"Zapamiętaj mnie", ponieważ nie zawsze ta funkcjonalność jest pożądana. Wykonuje się
to zwykle przez dodanie pola wyboru do formularza logowania. Gdy nada się temu
polu nazwę ``_remember_me`` (lub nazwę skonfigurowana w ``remember_me_parameter``),
bedzie automatycznie ustawiane ciasteczko, gdy pole wyboru jest zaznaczone i użytkownik
będzie automatycznie logowany. Tak więc, formularz logowania może wyglądać tak:

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        {# app/Resources/views/security/login.html.twig #}
        {% if error %}
            <div>{{ error.message }}</div>
        {% endif %}

        <form action="{{ path('login_check') }}" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="{{ last_username }}" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <input type="checkbox" id="remember_me" name="_remember_me" checked />
            <label for="remember_me">Keep me logged in</label>

            <input type="submit" name="login" />
        </form>

    .. code-block:: html+php
       :linenos:

        <!-- app/Resources/views/security/login.html.php -->
        <?php if ($error): ?>
            <div><?php echo $error->getMessage() ?></div>
        <?php endif ?>

        <form action="<?php echo $view['router']->generate('login_check') ?>" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username"
                   name="_username" value="<?php echo $last_username ?>" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <input type="checkbox" id="remember_me" name="_remember_me" checked />
            <label for="remember_me">Keep me logged in</label>

            <input type="submit" name="login" />
        </form>

W okresie ważności ciasteczka użytkownik bedzie logowany automatycznie przy
każdym odwiedzeniu strony.

Wymuszanie ponownego uwierzytelniania użytkownika przy dostępie do pewnych zasobów
----------------------------------------------------------------------------------

Gdy użytkownik powraca na witrynę, jest automatycznie uwierzytelniany na podstawie
informacji zawartej w ciasteczku funkcjonalności "zapamiętaj mnie". Umożliwia to
użytkownikowi na dostęp do chronionych zasobów, tak jakby użytkownik rzeczywiście
się zalogował zaraz po odwiedzeniu witryny.

Jednak, w niektórych przypadkach moze być konieczne, wymuszenie ponownego zalogowania
sie użytkownika, przed dostępem do niektórych zasobów. Na przykład, można zezwolić
"zapamiętanym" użytkownikom widzieć podstawowe informacje na koncie, ale wymagać
ponownego uwierzytelnienia przed próbą modyfikowania tych informacji.

Komponent "Security" udostenia łatwy sposób na wykonanie tego. Oprócz ról
jawnie przypisanych użytkownikom, automatycznie przydzielana jest im jedna z
następujących ról, w zależności od sposobu uwierzytelnienia:

.. index::
   single: role; IS_AUTHENTICATED_ANONYMOUSLY 


``IS_AUTHENTICATED_ANONYMOUSLY``
    Automatycznie przypisywana użytkownikowi, który znalazł się w chronionej
    przez zaporę części witryny, ale który ma nie aktualne poświadczenie. Jest
    to tylko możliwe, jeśli dozwolony jest dostęp anonimowy.

.. index::
   single: role; IS_AUTHENTICATED_REMEMBERED 

``IS_AUTHENTICATED_REMEMBERED``
    Automatycznie przypisywana użytkownikowi, który został uwierzytelniony poprzez
    ciasteczko "zapamietaj mnie".

.. index::
   single: role; IS_AUTHENTICATED_FULLY

``IS_AUTHENTICATED_FULLY``
    Automatycznie przypisywana użytkownikowi, który dostarczył swoje dane logowania
    podczas bieżącej sesji.

Można wykorzystywać te role bez ich jawnego przypisywania.

.. note::

    Jeśli ma się rolę ``IS_AUTHENTICATED_REMEMBERED``, to ma się też rolę
    ``IS_AUTHENTICATED_ANONYMOUSLY``. Jeśli ma się rolę ``IS_AUTHENTICATED_FULLY``,
    to ma się też dwie poprzednie role. Innymi słowami, role te reprezentują
    trzy poziomy zwiększania "mocy" uwierzytelniania.

Można wykorzystać te dodatkowe role dla bardziej szczegółowej kontroli dostępu
do pewnych części witryny. Na przykład, można chcieć, aby użytkownik miał dostęp
do podgladu konta na ``/account`` gdy jest uwierzytelniony przez ciasteczko a
mogł edytować te dane, tylko gdy zalogował się w bieżącej sesji, czyli uzyskał
rolę ``IS_AUTHENTICATED_FULLY``. Można to zrobić zabezpieczając określoną akcję
kontrolera przy uzyciu odpowiednich ról. Akcja edytowania w kontrolerze może
zostać zabezpieczona przez wykorzystanie kontekstu usługi.

W poniższym przykładzie, akcja jest dozwolona, tylko jeśłi użytkownik ma rolę
``IS_AUTHENTICATED_FULLY``.

.. code-block:: php
   :linenos:

    // ...
    use Symfony\Component\Security\Core\Exception\AccessDeniedException

    // ...
    public function editAction()
    {
        $this->denyAccessUnlessGranted('IS_AUTHENTICATED_FULLY');

        // ...
    }

Jeśłi aplikacja jest oparta na Symfony Standard Edition, można również zabezpieczyć
akcję za pomocą adnotacji:

.. code-block:: php
   :linenos:

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;

    /**
     * @Security("has_role('IS_AUTHENTICATED_FULLY')")
     */
    public function editAction($name)
    {
        // ...
    }

.. tip::

    Gdyby miało się również kontrolę dostępu w konfiguracji bezpieczeństwa, która
    wymaga, aby użytkownik miał rolę ``ROLE_USER`` w celu możliwości dostępu
    do obszaru konta, wtedy ma się następująca sytuację:

    * Jeśli nieuwierzytelniony (lub uwierzytelniony anonimowo) użytkownik próbuje
      uzyskać dostęp do obszaru konta, zostanie poproszony o uwierzytelnienie.

    * Po wprowadzeniu swojej nazwy i hasła, jeśli użytkownik ma rolę ``ROLE_USER``,
      przypisana mu w konfiguracji, uzyska on rolę ``IS_AUTHENTICATED_FULLY``
      i będzie mógł uzyskać dostęp do każdej strony w sekcji konta, w tym do
      akcji ``editAction``.

    * Jeśli zakończy sie sesja użytkownika, to gdy użytkownik ten powróci do witryny,
      będzie mógł uzyskać dostęp do każdej strony obszaru konta, z wyjatkiem strony
      edycji, bez koniecznosci ponownego logowania się. Jednak, gdy spróbuje
      uzyskać dostęp do akcji ``editAction``, będzie zmuszony do ponownego
      uwierzytelnienia się.

Więcej informacji o usługach zabezpieczajacych lub metodach działających w ten sposób,
można znaleźć w artykule :doc:`/cookbook/security/securing_services`.
