.. highlight:: php
   :linenothreshold: 2

.. index::
   single: bezpieczeństwo; zapora
   double: komponent Security; zapora

Zapora i uwierzytelnianie
=========================

Centralną częścią komponentu Security jest mechanizm autoryzacji. Jest on obsługiwany
przez instancję :class:`Symfony\\Component\\Security\\Core\\Authorization\\AuthorizationCheckerInterface`.
Po pomyślnym wykonaniu wszystkich kroków w procesie uwierzytelniania użytkownika,
można uruchomić mechanizm sprawdzania autoryzacji w celu sprawdzenia, czy uwierzytelniony
użytkownik ma parawo dostępu do określonej akcji lub zasobu aplikacji::

    use Symfony\Component\Security\Core\Authorization\AuthorizationChecker;
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;

    // instance of Symfony\Component\Security\Core\Authentication\Token\Storage\TokenStorageInterface
    $tokenStorage = ...;

    // instance of Symfony\Component\Security\Core\Authentication\AuthenticationManagerInterface
    $authenticationManager = ...;

    // instance of Symfony\Component\Security\Core\Authorization\AccessDecisionManagerInterface
    $accessDecisionManager = ...;

    $authorizationChecker = new AuthorizationChecker(
        $tokenStorage,
        $authenticationManager,
        $accessDecisionManager
    );

    // ... authenticate the user

    if (!$authorizationChecker->isGranted('ROLE_ADMIN')) {
        throw new AccessDeniedException();
    }

.. versionadded:: 2.6
    W Symfony 2.6, klasa :class:`Symfony\\Component\\Security\\Core\\SecurityContext`
    została podzielona na klasy
    :class:`Symfony\\Component\\Security\\Core\\Authorization\\AuthorizationChecker`
    i :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\Storage\\TokenStorage`.

.. note::

    Proszę przeczytać dedykowane rozdziały, aby dowiedzieć się więcej o
    :doc:`/components/security/authentication` i :doc:`/components/security/authorization`.

.. _firewall:

Zapora dla żądań HTTP
---------------------

Uwierzytelnianie użytkowników jest realizowane przez zaporę (*ang. firewall*).
Aplikacja może mieć wiele obszarów chronionych, więc zapora jest konfigurowana
z mapą tych obszarów. Mapa ta zawiera, dla każdego z takiego obszaru,
mechanizm dopasowywania żądań i kolekcję detektorów zdarzeń (*ang. listeners*).
Mechanizm dopasowywania żądań daje zaporze możliwość wykrywania, czy bieżące
żądanie dotyczy obszaru chronionego.
Detektory zdarzeń są następnie odpytywane, czy bieżące żądanie może zostać użyte
do uwierzytelnienia użytkownika::

    use Symfony\Component\Security\Http\FirewallMap;
    use Symfony\Component\HttpFoundation\RequestMatcher;
    use Symfony\Component\Security\Http\Firewall\ExceptionListener;

    $map = new FirewallMap();

    $requestMatcher = new RequestMatcher('^/secured-area/');

    // instances of Symfony\Component\Security\Http\Firewall\ListenerInterface
    $listeners = array(...);

    $exceptionListener = new ExceptionListener(...);

    $map->add($requestMatcher, $listeners, $exceptionListener);

Metoda ``$map->add`` ma trzy argumenty:

- ``$requestMatcher``
   
   Mechanizm dopasowywania żądań, obiekt klasy ``Symfony\Component\HttpFoundation\RequestMatcher``,
   odpowiedzialny za wykrycie, czy bieżące żądanie pasuje do któregoś obszaru
   chronionego, określonego w tym obiekcie.
   
- ``$listners``
  
  Tablica detektorów uwierzytelniania, odpowiedzialnych za uwierzytelnienie, patrz
  :ref:`firewall_listeners`.   

- ``$exceptionListener``
  
  Detektor wyjątku, uruchamiany, gdy któryś z powyższych detektorów zrzuci wyjątek.
  patrz :ref:`exception_listeners`.   

Mapa jest przekazywane do zapory jako pierwszy argument, wraz
z dyspozytorem zdarzeń (*ang. event dispatcher*), który jest używany w
klasie :class:`Symfony\\Component\\HttpKernel\\HttpKernel`::

    use Symfony\Component\Security\Http\Firewall;
    use Symfony\Component\HttpKernel\KernelEvents;

    // the EventDispatcher used by the HttpKernel
    $dispatcher = ...;

    $firewall = new Firewall($map, $dispatcher);

    $dispatcher->addListener(
        KernelEvents::REQUEST,
        array($firewall, 'onKernelRequest')
    );

Zapora jest rejestrowania do nasłuchu zdarzeń ``kernel.request``, które są
rozdysponowywane przez klasę HttpKernel na początku przetwarzania każdego
żądania. W ten sposób zapora może uniemożliwić, aby użytkownik przeszedł dalej,
niż jest to dozwolone.

.. _firewall_listeners:

Detektory uwierzytelniania
~~~~~~~~~~~~~~~~~~~~~~~~~~

Kiedy zapora zostanie powiadomiona o zdarzeniu ``kernel.request``, odpytuje
mapę zapory, czy żądanie pasuje do jakiegość z obszarów chronionych. Pierwszy
dopasowany do żądania obszar zwraca zestaw odpowiednich detektorów (z których każdy
implementuje interfejs
:class:`Symfony\\Component\\Security\\Http\\Firewall\\ListenerInterface`).
Detektory te są następnie odpytywane, w celu obsługi bieżącego żądania. W zasadzie
oznacza to: wykryj, czy bieżące żądanie zawiera jakąś informację, na podstawie
której użytkownik może zostać uwierzytelniony (na przykład, detektor podstawowego
uwierzytelniania HTTP sprawdza, czy żądanie ma nagłówek o nazwie ``PHP_AUTH_USER``).

.. _exception_listeners:

Detektor wyjątku
~~~~~~~~~~~~~~~~

Jeśli któryś z detektorów zdarzeń w zaporze zrzuci wyjątek
:class:`Symfony\\Component\\Security\\Core\\Exception\\AuthenticationException`,
to nastąpi przeskok do detektora wyjąku, który został przekazany do zapory podczas
dodawania obszarów chronionych.

Detektor wyjatku określa, co sie ma dziać po zrzuceniu wyjatku przez któryś z
detektorów uwierzytelniania, w oparciu o otrzymane w czasie tworzenia argumenty.  
Może on rozpocząć procedurę uwierzytelniania,, moze poprosić użytkownika o ponowne
przesłanie uwierzytelnień (gdy ma on być uwierzytelniony na podstawie ciasteczka
"remember-me") lub przekształcić wyjatek do klasy
:class:`Symfony\\Component\\HttpKernel\\Exception\\AccessDeniedHttpException`,
która ostatecznie doprowadza do odpowiedzi "HTTP/1.1 403: Access Denied".

Punkty wejścia
~~~~~~~~~~~~~~

Gdy użytkownik nie jest w ogóle uwierzytelniony (czyli gdy w magazynie
tokenów nie ma jesze tokenu), zostanie wywołany "punkt wejścia" zapory w celu
"rozpoczęcia" procesu uwierzytelniania. "Punkt wejścia" zapory, to klasa implementująca
interfejs :class:`Symfony\\Component\\Security\\Http\\EntryPoint\\AuthenticationEntryPointInterface`,
która ma tylko jedną metodę:
:method:`Symfony\\Component\\Security\\Http\\EntryPoint\\AuthenticationEntryPointInterface::start`.
Metoda ta otrzymuje od detektora wyjątku bieżący obiekt :class:`Symfony\\Component\\HttpFoundation\\Request`
i wyjątek, który został zrzucony.
Metoda ``start`` powinna zwracać obiekt :class:`Symfony\\Component\\HttpFoundation\\Response`.
Może to być, na przykład, strona zawierająca formularz logowania lub w przypadku
podstawowego uwierzytelniania HTTP odpowiedź z nagłówkiem ``WWW-Authenticate``,
która prosi użytkownika o podanie nazwy użytkownika i hasła.

Przepływ: zapora, uwierzytelnianie, autoryzacja
-----------------------------------------------

Teraz, możemy przystąpić omówienia przepływu przetwarzania w kontekście bezpieczeństwa:

#. Zapora jest rejestrowana do nasłuchu zdarzenia ``kernel.request``;
#. Na początku przetwarzania żądania, zapora sprawdza, czy mapa zapory powinna
   być aktywna dla tej ścieżki URL;
#. Jeśli dla danego URL zapora została odnaleziona na mapie, powiadamiane są
   jej detektory uwierzytelniania;
#. Każdy detektor uwierzytelniania sprawdza, czy bieżące żądanie zawiera jakieś
   informacje uwierzytelniajace - detektor może: (a) uwierzytelnić użytkownika,
   (b) zrzucić wyjątek ``AuthenticationException`` lub (c) nic nie zrobić
   (ponieważ brak jest jakiejkolwiek informacji uwierzytelniającej w żądaniu);
#. Po uwierzytelnieniu uzytkownika można uruchomic mechanizm
   :doc:`autoryzacji </components/security/authorization>` w celu udzielenia dostępu
   do określonych zasobów.

Proszę przeczytac nastęþny rozdział, w celu poznania więcej informacji
o :doc:`uwierzytelnianiu </components/security/authentication>`
i :doc:`autoryzacji </components/security/authorization>`.
