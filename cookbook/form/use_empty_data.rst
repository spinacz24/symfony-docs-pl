.. index::
   single: Form; Puste dane empty_data

Konfiguracja opcji empty_data
============================================

Opcja empty_data pozwala określić wstępny zbiór danych dla klasy formularza. 
Wstępny zbór danych zostanie użyty jeżeli Twój formularz zostanie wysłany, ale nie zostanie
na nim wywołana metoda ``setData()``. Dla przykładu::

    public function indexAction()
    {
        $blog = ...;

        // zmienna $blog przekazywana jest jako dane, dlatego opcja empty_data
        // nie będzie tu użyta
        $form = $this->createForm(new BlogType(), $blog);

        // żadne dane nie są przekazywane, więc zostanie użyta opcja empty_data
        // aby wprowadzić wstępny zbór danych
        $form = $this->createForm(new BlogType());
    }

Domyślnie, opcja ``empty_data`` ma przypisaną wartość null. Jeżeli masz ustawioną opcję
``data_class`` w twojej klasie formularza to w ``empty_data`` domyślnie będzie 
nowa instancja tej klasy. Ta instancja będzie utworzona wraz wywołaniem konstruktora ale *bez argumentów*.

Są dwa sposoby na zmianę tego zachowania.

Sposób 1: Utworzenie nowej instancji klasy
---------------------------------

Jest jeden powód dlaczego chciałbyś używać ten sposób, mianowicie pozwala on przekazać do
konstruktora klasy parametry. Pamiętaj domyślnie opcja ``data_class`` wywołuje konstruktor
*bez parametrów*::

    // src/AppBundle/Form/Type/BlogType.php

    // ...
    use Symfony\Component\Form\AbstractType;
    use AppBundle\Entity\Blog;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class BlogType extends AbstractType
    {
        private $someDependency;

        public function __construct($someDependency)
        {
            $this->someDependency = $someDependency;
        }
        // ...

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'empty_data' => new Blog($this->someDependency),
            ));
        }
    }


Możesz zainicjować klasę jak tylko chcesz. W tym przykładzie, podczas inicjalizacji przekazujemy pewne
zależności do ``BlogType`` a następnie używamy je aby utworzyć obiekt klasy ``Blog``.
Chodzi o to, można ustawić empty_data do konkretnego "nowego" obiektu, który chcesz użyć.

Sposób 2: Użycie Funkcji anonimowej
---------------------------

Używanie funkcji anonimowej jest preferowaną metodą, ponieważ stworzy obiekt tylko
jeżeli będzie potrzebny.

Funkcja anonimowa musi jako pierwszy argument przekazać instancję która 
implementuje interfejs ``FormInterface``::

    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Form\FormInterface;
    // ...

    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults(array(
            'empty_data' => function (FormInterface $form) {
                return new Blog($form->get('title')->getData());
            },
        ));
    }