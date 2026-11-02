//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: delegate.h                            //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef DELEGATE_H
#define DELEGATE_H

#include <cstddef>

// Schlanker Ersatz für std::function + std::bind in den heißen Pfaden der Emulation
// (Speicherzugriffe, IO, Interrupt-Leitungen).
// Ein Delegate besteht nur aus dem Objektzeiger und einem Funktionszeiger auf eine kleine Hilfsfunktion (Thunk),
// die die Member-Funktion direkt aufruft. Ein Aufruf kostet damit genau einen indirekten Sprung,
// das Kopieren (z.B. beim Umbauen der Speichertabellen) nur zwei Zeiger.
//
// Binden:  proc = BindDelegate<&Klasse::Methode>(objekt);
// Aufruf:  proc(argumente);   Prüfen: if(proc != nullptr)

template<typename Signature> class Delegate;

template<typename R, typename... Args>
class Delegate<R(Args...)>
{
public:
    Delegate() : obj(nullptr), fn(nullptr) {}
    Delegate(std::nullptr_t) : obj(nullptr), fn(nullptr) {}

    template<auto Method, typename T>
    static Delegate Bind(T *object)
    {
        Delegate d;
        d.obj = object;
        d.fn = &Thunk<Method, T>;
        return d;
    }

    inline R operator()(Args... args) const { return fn(obj, args...); }

    bool operator==(std::nullptr_t) const { return fn == nullptr; }
    bool operator!=(std::nullptr_t) const { return fn != nullptr; }
    explicit operator bool() const { return fn != nullptr; }

private:
    template<auto Method, typename T>
    static R Thunk(void *object, Args... args)
    {
        return (static_cast<T*>(object)->*Method)(args...);
    }

    void *obj;
    R (*fn)(void*, Args...);
};

// Hilfsobjekt, damit die Signatur beim Zuweisen aus dem Ziel-Delegate abgeleitet wird
template<auto Method, typename T>
struct DelegateBinder
{
    T *object;

    template<typename Signature>
    operator Delegate<Signature>() const
    {
        return Delegate<Signature>::template Bind<Method>(object);
    }
};

template<auto Method, typename T>
inline DelegateBinder<Method, T> BindDelegate(T *object)
{
    return DelegateBinder<Method, T>{object};
}

#endif // DELEGATE_H
