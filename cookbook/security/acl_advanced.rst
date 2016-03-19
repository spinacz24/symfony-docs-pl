.. index::
   single: bezpieczeństwo; ACL
   single: autoryzacja; ACL
   single: lista kontroli dostępu (ACL); koncepcje
   single: ACL
   
Jak wykorzystywać zaawansowane koncepcje ACL
============================================

Celem niniejszego artykułu jest danie pogłębionego obrazu systemu ACL oraz
wyjaśnienie kilku decyzji projektowych stojących za tym systemem.

Koncepcje projektowe
--------------------

Możliwość zabezpieczania obiektów w Symfony opiera się na koncepcji list kontroli
dostępu (*ang. Access Control List - ACL*). Każdy obiekt domeny ma swoja własną
listę ACL. Poszczególne egzemplarze ACL przechowują szczegółowe wpisy kontroli
dostępu (*ang. Access Control Entries - ACE*), które są wykorzystywane przy podejmowaniu
decyzji o dostępie. System ACL Symfony koncentruje się na dwóch głównych celach:

- zapewnienie sposobu na pobieranie dużej ilości list ACL i wpisów ACE dla obiektów
  domeny i ich modyfikacji;
- zapewnienie sposobu na łatwe podejmowanie decyzji, czy jakaś osoba może wykonać
  jakąś akcję na obiekcie domeny.

Jak już otym wspomniano, jednym z głównych możliwości systemu ACL jest zapewnienie
wysokiej wydajności sposobu pobierania list ACL i wpisów ACE. Jest to niezwykle
ważne, ponieważ każda lista ACL może mieć kilka wpisów ACE i dziedziczyć z innych
list ACL w trybie drzewiastej. Dlatego system ACL nie opiera się żadnym mechaniźmie
ORM. Domyślna implementacja wykorzystuje połączenie z DBAL Doctrine.

Tożsamości obiektów
~~~~~~~~~~~~~~~~~~~

System ACL jest całkowicie oddzielony od obiektów domeny. Nie muszą być one
nawet przechowywane w tej samej bazie danych lub na tym samym serwerze. W celu
spełnienia tego oddzielenia, obiekty w systemie ACL są reprezentowane przez 
obiekt tożasmosci obiektu. Za każdym razem, gdy chce się pobrać listę ACL dla
obiektu domeny, system ACL najpierw utworzy tożsamość obiektu z obiektu
domeny i następnie przekaże tą tozsamość do dostawcy ACL w celu dalszego
przetwarzania.

Tożsamości bezpieczeństwa
~~~~~~~~~~~~~~~~~~~~~~~~~

Jest to analogiczne do tożsamość obiektu, ale reprezentuje użytkownika lub
rolę w aplikacji. Każda rola lub użytkownik mają własną tozsamość bezpieczeństwa.

.. caution::

    W przypadku użytkowników, tożsamość bezpieczeństwa jest oparta na nazwie
    użytkownika. Oznacza to, że jeżeli z jakichś powodów zostanie zmieniona nazwa
    uzytkownika, trzeba również zaktualizowac tozsamość bezpieczeństwa.
    Do tego celu jest przeznaczona metoda
    :method:`MutableAclProvider::updateUserSecurityIdentity() <Symfony\\Component\\Security\\Acl\\Dbal\\MutableAclProvider::updateUserSecurityIdentity>`.
   
Struktura tabel bazy danych
---------------------------

Domyślna implementacja używa pięć tabel bazy danych, jak wymieniono poniżej:

- *acl_security_identities*: Tabela rejestrująca wszystkie tozsamości
  bezpieczeństwa (SID), które utrzymują wpisy ACE. Domyślna implementacja
  dostarcza dwie tozsamości bezpieczeństwa:
  
  - :class:`Symfony\\Component\\Security\\Acl\\Domain\\RoleSecurityIdentity`,
  - :class:`Symfony\\Component\\Security\\Acl\\Domain\\UserSecurityIdentity`.

- *acl_classes*: Tabela odwzorowująca nazwy klas na unikalny ID, do którego
  można odwoływać się z innych tabel.
- *acl_object_identities*: Każdy wiersz w tej tabeli reprezentuje pojenczy obiekt
  domeny.
- *acl_object_identity_ancestors*: Tabela rejestrująca elementy nadrzędne ACL,
  co umożliwia budowę drzewiastej struktury list ACL. 
- *acl_entries*: Tabela zawierająca wszystkie wpisy ACE. Jest to typowa tabela
  z najwiekszą ilościa wierszy. Może zawierać dziesiątki milionów wierszy, bez
  znacznego wpływu na wydajność.

.. _cookbook-security-acl-field_scope:

Zakres wpisów kontroli dostępu
------------------------------

Wpisy kontroli dostępu (ACE) moga mieć różny zakres stosowania. W Symfony, są dwa
podstawowe zakresy:

- Class-Scope: Wpisy te stosowane są dla wszystkich obiektów tej samej klasy.
- Object-Scope: Zakres ten został użyty jedynie w poprzednim rozdziale i ma
  zastosowanie tylko dla określonego obiektu.

Czasami występuje potrzeba zastosowania wpisu ACE tylko dla określonego pola
obiektu. Załóżmy, że chcemy, aby ID był widoczny tylko dla administratora,
ale nie przez obsługę klienta. Dla rozwiązania tego problemu, trzeba dodać dwa
podzakresy:

- Class-Field-Scope: Wpisy te stosuje sie do wszystkich obiektów tej samej klasy,
  ale dla określonego pola tych obiektów.
- Object-Field-Scope: Wpis ten stosuje sie do określonego obiektu, ale tylko dla
  określonego pola tego obiektu.

Decyzje wczesnej autoryzacji
----------------------------

Dla decyzji wczesnej autoryzacji, czyli decyzji podejmowanych przed wywołaniem
każdej bezpiecznej metody, stosowana jest usługa ``AccessDecisionManager``.
Usługa ta jest również używana dla do podejmowania decyzji opartych
na rolach. System ACL dodaje kilka nowych atrybutów, które moga być użyte do
sprawdzania różnych uprawnień.

Wbudowana mapa uprawnień
~~~~~~~~~~~~~~~~~~~~~~~~

+----------+-------------------------------+----------------------------+
| Atrybut  | Znaczenie                     | Bitowa maska bitowa        |
+==========+===============================+============================+
| VIEW     | Czy ktoś może oglądać         | VIEW, EDIT, OPERATOR,      |
|          | obiekt domeny.                | MASTER lub OWNER           |
+----------+-------------------------------+----------------------------+
| EDIT     | Czy ktoś może wprowadzać      | EDIT, OPERATOR, MASTER     |
|          | zmiany do obiektu domeny.     | lub OWNER                  |
+----------+-------------------------------+----------------------------+
| CREATE   | Czy ktoś może tworzyć         | CREATE, OPERATOR, MASTER   |
|          | obiekt domeny.                | lub OWNER                  |
+----------+-------------------------------+----------------------------+
| DELETE   | Czy ktoś może usuwać          | DELETE, OPERATOR, MASTER   |
|          | obiekt domeny.                | lub OWNER                  |
+----------+-------------------------------+----------------------------+
| UNDELETE | Czy ktoś może przywracać      | UNDELETE, OPERATOR, MASTER |
|          | poprzednio usunięty           | lub OWNER                  |
|          | obiekt domeny.                |                            |
+----------+-------------------------------+----------------------------+
| OPERATOR | Czy ktoś może wykonywać       | OPERATOR, MASTER lub OWNER |
|          | wszystkie powyższe akcje.     |                            |
+----------+-------------------------------+----------------------------+
| MASTER   | Czy ktoś może wykonywać       | MASTER lub OWNER           |
|          | wszystkie powyższe akcje      |                            |
|          | i dodatkowo jest upoważniony  |                            |
|          | przydzielania innym           |                            |
|          | wszystkich powyższych         |                            |
|          | uprawnień.                    |                            |
+----------+-------------------------------+----------------------------+
| OWNER    | Czy ktoś jest właścicielem    | OWNER                      |
|          | obiektu domeny. Właściciel    |                            |
|          | może wykonywać każdą powyższą |                            |
|          | akcję i przydzielać główne    |                            |
|          | i własnościowe uprawnienia.   |                            |
+----------+-------------------------------+----------------------------+

Atrybuty uprawnień vs. mapy bitowe uprawnień
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Atrybuty są używane przez AccessDecisionManager, podobnie jak role. Często,
atrybuty te reprezentują w rzeczywistości agregat liczbowych masek bitowych.
Z drugiej strony, liczbowe maski bitowe są używane wewnętrznie przez system
ACL do efektywnego przechowywania uprawnień użytkownika w bazie danych
i sprawdzania dostępu przy użyciu bardzo szybkich operacji na maskach bitowych.

Rozszerzanie
~~~~~~~~~~~~

Powyższa mapa uprawnień nie jest statyczna i teoretycznie może zostać całkowicie
wymieniona. Należy jednak starać sie pokryć jak najwięcej napotkanych problemów
i współdziałać z innymi pakietami.

Decyzje po autoryzacyjne
------------------------

Decyzje po autoryzacyjne są wykonywane po wykonaniu bezpiecznych metod i 
i zazwyczaj dotyczy obiektu domeny, który jest zwracany przez ta metodę.
Po wywołaniu, dostawcy pozwalają również modyfikować lub filtrować obiekt domeny
przed jego zwróceniem.

Z powodu ograniczeń języka PHP, nie ma możliwość budowania po autoryzacyjnego
w rdzennym komponencie Security.
Jednakże, istnieje ekperymentalny pakiet `JMSSecurityExtraBundle`_, który daje
takie możliwości. Proszę zapoznać sie z dokumentacją tego pakietu.

Proces podejmowania decyzji autoryzacyjnych
-------------------------------------------

Klasa ACL dostarcza dwie metody dla określania, czy tożsamość bezpieczeństwa
ma wymagane maski bitowe: ``isGranted`` i ``isFieldGranted``. Gdy ACL otrzymuje
żądanie autoryzacji za pomocą jednej z tych metod, deleguje to żądanie do
implementacji
:class:`Symfony\\Component\\Security\\Acl\\Domain\\PermissionGrantingStrategy`.
Pozwala to zastąpić sposób podejmowania decyzji o dostępie bez faktycznego
modyfikowania klasy ACL.

Obiekt klasy ``PermissionGrantingStrategy`` sprawdza najpierw wszystkie wpisy ACE
w zakresie obiektu. Jeśli żaden wpis nie ma zastosowania, to przetwarzanie zostanie 
powtórzone dla wpisów ACE nadrzędnej listy ACL. Jeśli taka lista nie istnieje,
zrzucony zostanie wyjątek.

.. _`JMSSecurityExtraBundle`: https://github.com/schmittjoh/JMSSecurityExtraBundle
