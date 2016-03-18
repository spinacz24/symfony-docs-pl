.. highlight:: php
   :linenothreshold: 2

.. index::
   single: bezpieczeństwo; uwierzytelnianie
   double: komponent Security; uwierzytelnianie
   single: uwierzytelnianie; magazyn tokenów

Uwierzytelnianie
================

Gdy żądanie wskazuje na obszar chroniony i jeden z detektorów z mapy zapory
jest w stanie wyodrębnić z bieżącego obiektu
:class:`Symfony\\Component\\HttpFoundation\\Request` poświadczenia użytkownika,
powinien zostać utworzony token, zawierający te poświadczenia. Następną rzeczą,
która powinien zrobić detektor, jest zapytanie menadżera uwierzytelniania o
potwierdzenie tego tokenu i zwrócenie *uwierzytelnionego tokenu*, jeśli okaże się,
że podane poświadczenie jest prawidłowe.
Detektor powinien następnie zapisać uwierzytelniony token, wykorzystując 
:class:`magazyn tokenów <Symfony\\Component\\Security\\Core\\Authentication\\Token\\Storage\\TokenStorageInterface>`::

    use Symfony\Component\Security\Http\Firewall\ListenerInterface;
    use Symfony\Component\Security\Core\Authentication\Token\Storage\TokenStorageInterface;
    use Symfony\Component\Security\Core\Authentication\AuthenticationManagerInterface;
    use Symfony\Component\HttpKernel\Event\GetResponseEvent;
    use Symfony\Component\Security\Core\Authentication\Token\UsernamePasswordToken;

    class SomeAuthenticationListener implements ListenerInterface
    {
        /**
         * @var TokenStorageInterface
         */
        private $tokenStorage;

        /**
         * @var AuthenticationManagerInterface
         */
        private $authenticationManager;

        /**
         * @var string Uniquely identifies the secured area
         */
        private $providerKey;

        // ...

        public function handle(GetResponseEvent $event)
        {
            $request = $event->getRequest();

            $username = ...;
            $password = ...;

            $unauthenticatedToken = new UsernamePasswordToken(
                $username,
                $password,
                $this->providerKey
            );

            $authenticatedToken = $this
                ->authenticationManager
                ->authenticate($unauthenticatedToken);

            $this->tokenStorage->setToken($authenticatedToken);
        }
    }

.. note::

    Token może być obiektem jakiejkolwiek klasy, pod warunkiem, że implementuje
    :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface`.

.. index::
   single: uwierzytelnianie; menadżer uwierzytelniania

Menadżer uwierzytelniania
-------------------------

Domyślny menadżer uwierzytelniania jest instancją klasy
:class:`Symfony\\Component\\Security\\Core\\Authentication\\AuthenticationProviderManager`::

    use Symfony\Component\Security\Core\Authentication\AuthenticationProviderManager;

    // instances of Symfony\Component\Security\Core\Authentication\Provider\AuthenticationProviderInterface
    $providers = array(...);

    $authenticationManager = new AuthenticationProviderManager($providers);

    try {
        $authenticatedToken = $authenticationManager
            ->authenticate($unauthenticatedToken);
    } catch (AuthenticationException $failed) {
        // authentication failed
    }

Podczas tworzona instancji ``AuthenticationProviderManager``, odbiera ona kilka
wystawców uwierzytelniania, z których każdy obsługuje różne rodzaje tokenów.

.. note::

    Można oczywiście napisać własnego menadżera uwierzytelniania, wystarczy tylko
    zaimplementowac :class:`Symfony\\Component\\Security\\Core\\Authentication\\AuthenticationManagerInterface`.

.. index::
   single: uwierzytelnianie; wystawca uwierzytelniania

.. _authentication_providers:

Wystawcy uwierzytelniania
-------------------------

Każdy wystawca
ma metodę :method:`Symfony\\Component\\Security\\Core\\Authentication\\Provider\\AuthenticationProviderInterface::supports`,
ponieważ implementuje interfejs
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Provider\\AuthenticationProviderInterface`,
co umożliwia, aby ``AuthenticationProviderManager`` mógł ustalać, czy ma obsługiwać 
dany token. W takim przypadku, menadżer wywołuje metodę wystawcy
:method:`Symfony\\Component\\Security\\Core\\Authentication\\Provider\\AuthenticationProviderInterface::authenticate`.
Metoda ta powinna zwracać uwierzytelniony token lub zrzucać wyjątek
:class:`Symfony\\Component\\Security\\Core\\Exception\\AuthenticationException`
(lub jakiś inny wyjątek go rozszrzający).

Uwierzytelnianie użytkowników poprzez ich nazwę i hasło
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Wystawca uwierzytelniania podejmuje próbę uwierzytelnienia użytkownika na podstawie
poświadczeń przez niego dostarczonych. Zwykle są nimi nazwa użytkownika i hasło.
Większość aplikacji przechowuje nazwę użytkownika i zaszyfrowane hasło użytkownika
zmieszane z losowo wygenerowaną solą. Oznacza to, że przechowywane uwierzytelnianie
składa się z pobranej soli i zaszyfrowanego hasła. Jest ono porównywane z co dopiero
dostarczonym hasłem użytkownika (np. za pomocą formularza logowania), w celu
sprawdzenie jego poprawności.

Funkcjonalność ta jest oferowana przez klasę
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Provider\\DaoAuthenticationProvider`.
Pobiera ona dane użytkowników z :class:`Symfony\\Component\\Security\\Core\\User\\UserProviderInterface`,
wykorzystując :class:`Symfony\\Component\\Security\\Core\\Encoder\\PasswordEncoderInterface`
do tworzenia szyfrowanego hasła i zwracania uwierzytelnionego tokenu, jeśli hasło
jest prawidłowe::

    use Symfony\Component\Security\Core\Authentication\Provider\DaoAuthenticationProvider;
    use Symfony\Component\Security\Core\User\UserChecker;
    use Symfony\Component\Security\Core\User\InMemoryUserProvider;
    use Symfony\Component\Security\Core\Encoder\EncoderFactory;

    $userProvider = new InMemoryUserProvider(
        array(
            'admin' => array(
                // hasłem jest "foo"
                'password' => '5FZ2Z8QIkA7UTZ4BYkoC+GsReLf569mSKDsfods6LYQ8t+a8EW9oaircfMpmaLbPBh4FOBiiFyLfuZmTSUwzZg==',
                'roles'    => array('ROLE_ADMIN'),
            ),
        )
    );

    // dla dodatkowego sprawdzenia: czy konto jest włączone, zablokowane, wygasłe itd.?
    $userChecker = new UserChecker();

    // tabela koderów haseł (patrz niżej)
    $encoderFactory = new EncoderFactory(...);

    $provider = new DaoAuthenticationProvider(
        $userProvider,
        $userChecker,
        'secured_area',
        $encoderFactory
    );

    $provider->authenticate($unauthenticatedToken);

.. note::

    W powyższym przykładzie użyto dostawcę użytkownika "in-memory",
    ale można zastosować jakiegokolwiek innego dostawcę użytkowników, o ile implementuje on
    :class:`Symfony\\Component\\Security\\Core\\User\\UserProviderInterface`.
    Możliwe jest też, aby wielu dostawców użytkowników mogło próbować odnaleźć
    dane użytkownika, używając
    :class:`Symfony\\Component\\Security\\Core\\User\\ChainUserProvider`.

Wytwórnia szyfrowanych haseł
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Klasa :class:`Symfony\\Component\\Security\\Core\\Authentication\\Provider\\DaoAuthenticationProvider`
wykorzystuje "wytwórnię szyfrującą" do tworzenia szyfrowanych haseł dla określonego
typu użytkowników. Dzięki temu można stosować różne strategie szyfrowania dla
różnych typów użytkowników. Domyślnie :class:`Symfony\\Component\\Security\\Core\\Encoder\\EncoderFactory`
odbiera tablicę koderów::

    use Symfony\Component\Security\Core\Encoder\EncoderFactory;
    use Symfony\Component\Security\Core\Encoder\MessageDigestPasswordEncoder;

    $defaultEncoder = new MessageDigestPasswordEncoder('sha512', true, 5000);
    $weakEncoder = new MessageDigestPasswordEncoder('md5', true, 1);

    $encoders = array(
        'Symfony\\Component\\Security\\Core\\User\\User' => $defaultEncoder,
        'Acme\\Entity\\LegacyUser'                       => $weakEncoder,

        // ...
    );

    $encoderFactory = new EncoderFactory($encoders);

Każdy koder powinien implementować interfejs
:class:`Symfony\\Component\\Security\\Core\\Encoder\\PasswordEncoderInterface`
lub być tablicą kluczy ``class`` i ``arguments``, co pozwala wytwórni szyfrujacej
konstruoawć koder, tylko jeśli jest on potrzebny.

Tworzenie własnego kodera haseł
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W Symfony wbudowanych jest wiele koderów haseł, ale jeśli potrzeba stworzyć własny
koder, trzeba przestrzegać następujące zasady:

#. Klasa musi implementować :class:`Symfony\\Component\\Security\\Core\\Encoder\\PasswordEncoderInterface`;

#. Implementując metody
   :method:`Symfony\\Component\\Security\\Core\\Encoder\\PasswordEncoderInterface::encodePassword`
   i
   :method:`Symfony\\Component\\Security\\Core\\Encoder\\PasswordEncoderInterface::isPasswordValid`
   trzeba najpierw sprawdzic, czy hasło nie jest za długie, czyli że długość hasła
   nie przekracza 4096 znaków. Związane jest to względami bezpieczeństwa (zobacz `CVE-2013-5750`_)
   i do sprawdzenia tego można używać metodę
   :method:`Symfony\\Component\\Security\\Core\\Encoder\\BasePasswordEncoder::isPasswordTooLong`::

       use Symfony\Component\Security\Core\Encoder\BasePasswordEncoder;
       use Symfony\Component\Security\Core\Exception\BadCredentialsException;

       class FoobarEncoder extends BasePasswordEncoder
       {
           public function encodePassword($raw, $salt)
           {
               if ($this->isPasswordTooLong($raw)) {
                   throw new BadCredentialsException('Invalid password.');
               }

               // ...
           }

           public function isPasswordValid($encoded, $raw, $salt)
           {
               if ($this->isPasswordTooLong($raw)) {
                   return false;
               }

               // ...
       }

Używanie szyfrowanych haseł
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Kiedy wywoływana jest metoda wytwórni szyfrowania haseł
:method:`Symfony\\Component\\Security\\Core\\Encoder\\EncoderFactory::getEncoder`
z obiektem uzytkownika, przekazanym w pierwszym argumencie, zwrócony będzie koder
typu :class:`Symfony\\Component\\Security\\Core\\Encoder\\PasswordEncoderInterface`,
który powinien być zastosowany do kodowanie tego hasła użytkownika::

    // a Acme\Entity\LegacyUser instance
    $user = ...;

    // the password that was submitted, e.g. when registering
    $plainPassword = ...;

    $encoder = $encoderFactory->getEncoder($user);

    // will return $weakEncoder (see above)
    $encodedPassword = $encoder->encodePassword($plainPassword, $user->getSalt());

    $user->setPassword($encodedPassword);

    // ... save the user

Teraz, gdy bedzie sie chciało sprawdzić (np. w czasie próby logowania), czy
przesłane hasło jest właściwe, można użyć::

    // fetch the Acme\Entity\LegacyUser
    $user = ...;

    // the submitted password, e.g. from the login form
    $plainPassword = ...;

    $validPassword = $encoder->isPasswordValid(
        $user->getPassword(), // the encoded password
        $plainPassword,       // the submitted password
        $user->getSalt()
    );

Zdarzenia uwierzytelniania
--------------------------

Komponent Security dostarcza 4 zdarzenia związane z uwierzytelnianiem:

===============================  ================================================  ==============================================================================
Nazwa                            Stała zdarzenia                                   Argument przekazywany do detektora uwierzytelniania
===============================  ================================================  ==============================================================================
security.authentication.success  ``AuthenticationEvents::AUTHENTICATION_SUCCESS``  :class:`Symfony\\Component\\Security\\Core\\Event\\AuthenticationEvent`
security.authentication.failure  ``AuthenticationEvents::AUTHENTICATION_FAILURE``  :class:`Symfony\\Component\\Security\\Core\\Event\\AuthenticationFailureEvent`
security.interactive_login       ``SecurityEvents::INTERACTIVE_LOGIN``             :class:`Symfony\\Component\\Security\\Http\\Event\\InteractiveLoginEvent`
security.switch_user             ``SecurityEvents::SWITCH_USER``                   :class:`Symfony\\Component\\Security\\Http\\Event\\SwitchUserEvent`
===============================  ================================================  ==============================================================================

Zdarzenia związane z powodzeniem i niepowodzeniem uwierzytelniania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Gdy wystawca uwierzytelniania uwierzytelnia użytkownika, wysyłane jest zdarzenie
``security.authentication.success``. Lecz uwaga - zdarzenie to będzie wyzwalane,
na przykład, dla każdego żądania, jeśli ma sie uwierzytelnianie oparte na sesji.
Proszę zapoznać się ze zdarzeniem ``security.interactive_login``, jeśli zachodzi
potrzeba wykonania czegoś, gdy użytkownik jest już zalogowany.

W przypadku, gdy wystawca podejmie próbę uwierzytelnienia, ale zakończy się ono
niepowodzeniem (np. zrzucony zostanie wyjątek ``AuthenticationException``),
wysłane będzie zdarzenie ``security.authentication.failure``. Można nasłuchiwać
zdarzenia ``security.authentication.failure``, na przykład, w celu rejestrowania
nieudanych prób logowania.

Zdarzenia bezpieczeństwa
~~~~~~~~~~~~~~~~~~~~~~~~

Zdarzenie ``security.interactive_login`` jest wyzwalane po tym, jak użytkownik
został interaktywnie zalogowny na witrynie. Ważne jest, aby odróżnić to działanie
od metod nie interaktywnego uwierzytelniania, takich jak:

* uwierzytelninie oparte na ciasteczku "remember me",
* uwierzytelnianie oparte na sesji,
* uwierzytelnianie używajace nagłówków podstawowego uwierzytelniania HTTP lub
  Digest HTTP.

Zdarzenie ``security.interactive_login`` może byc nasłuchiwane, na przykład,
w celu przesłania użytkownikowi komunikatu powitalnego po każdym udanym logowaniu.

Zdarzenie ``security.switch_user`` jest wyzwalane zawsze, gdy aktywowany jest
detektor zapory ``switch_user``.

.. seealso::

    Więcej informacji mozna znależć w artykule
    :doc:`/cookbook/security/impersonating_user`.

.. _`CVE-2013-5750`: https://symfony.com/blog/cve-2013-5750-security-issue-in-fosuserbundle-login-form
.. _`BasePasswordEncoder::checkPasswordLength`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Security/Core/Encoder/BasePasswordEncoder.php
