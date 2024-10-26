//import { initializeApp } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-app.js";
//import { getFirestore, setDoc, collection, doc, getDocs, query, where, orderBy, limit } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-firestore.js";

import { initializeApp } from "firebase/app";
import { getFirestore, setDoc, collection, doc, getDocs, query, where, orderBy, limit } from "firebase/firestore";

const firebaseConfig = {
  apiKey: "AIzaSyA2nl8t6BDKGqyg_f1hxlRcC9v6DuwVkJg",
  authDomain: "saida-rapida.firebaseapp.com",
  projectId: "saida-rapida",
  storageBucket: "saida-rapida.appspot.com",
  messagingSenderId: "526559004556",
  appId: "1:526559004556:web:8fe92202bea42019e8b8a9"
};

const app = initializeApp(firebaseConfig);
const db = getFirestore();


//const buttonEnter = document.getElementById("entrar");
//const buttonExit = document.getElementById("sair");
//const text = document.getElementById("text");

// KatraK
class Catraca {
  constructor(escola) {
    this.date = new Date();
    this.col = collection(db, "escolas", escola, "historico")
  }

  getDate(IDAluno) {
    const year = this.date.getFullYear()
    const month = this.date.getMonth()
    const date = this.date.getDate()
    const hour = this.date.getHours()
    const minutes = this.date.getMinutes()
    const seconds = this.date.getSeconds()
    console.log(`dia: ${date}, mês: ${month}, ano: ${year}, hora: ${hour}, minuto ${minutes}, segundo: ${seconds}`)
    return String(`${date}${month}${year}${hour}${minutes}${seconds}${IDAluno}`)
  }

  async filtrar(IDAluno) {
    const q = query(this.col, where("aluno", "==", IDAluno), orderBy("hora", "desc"), limit(1))
    const querySnapshot = await getDocs(q); //pega os documentos filtrados

    let status = "foraDaEscola"

    if (!querySnapshot.empty) {
      const doc = querySnapshot.docs[0]
      status = doc.get("status")
    } 
    return status
  }

  async NaEscola(IDAluno, autorizadoPor) {
    const status = await this.filtrar(IDAluno)

    //if (status != "foraDaEscola") throw new Error(`${IDAluno} ainda não saiu da escola, o status atual é ${status}`)
    
    const IDRegistro = this.getDate(IDAluno)
    
    await setDoc(doc(this.col, IDRegistro), {
      aluno: IDAluno,
      hora: this.date,
      autorizadoPor: autorizadoPor,
      status: "naEscola"
    });
  
    console.log("Enter registred with success.")
  }

  async NaSala(IDAluno, autorizadoPor) {
    const status = await this.filtrar(IDAluno)

    //if (status != "naEscola") throw new Error(`${IDAluno} ainda não entrou na escola, o status atual é ${status}`)
    
    const IDRegistro = this.getDate(IDAluno)
  
    await setDoc(doc(this.col, IDRegistro), {
      aluno: IDAluno,
      hora: this.date,
      autorizadoPor: autorizadoPor,
      status: "naSala"
    });
  
    console.log("Enter at classroom registred with success.")
  }

  async Autorizado(IDAluno, autorizadoPor) {
    const status = await this.filtrar(IDAluno)

    //if (status != "naSala") throw new Error(`${IDAluno} ainda não entrou na sala, o status atual é ${status}`)
    
    const IDRegistro = this.getDate(IDAluno)
  
    await setDoc(doc(this.col, IDRegistro), {
      aluno: IDAluno,
      hora: this.date,
      autorizadoPor: autorizadoPor,
      status: "autorizado"
    });
  
    console.log("Student authorized with success.")
  }

  async EmTransicao(IDAluno, autorizadoPor) {
    const status = await this.filtrar(IDAluno)

    //if (status != "autorizado") throw new Error(`${IDAluno} ainda não entrou foi autorizado, o status atual é ${status}`)

    const IDRegistro = this.getDate(IDAluno)

    await setDoc(doc(this.col, IDRegistro), {
      aluno: IDAluno,
      hora: this.date,
      autorizadoPor: autorizadoPor,
      status: "emTransicao"
    });

    console.log("Transition registred with success.")
  }

  async ForaDaEscola(IDAluno, autorizadoPor) {
    const status = await this.filtrar(IDAluno)

    //if (status != "emTransicao") throw new Error(`${IDAluno} aluno ainda nao esta em transicao, o status atual é ${status}`)

    const IDRegistro = this.getDate(IDAluno)

    await setDoc(doc(this.col, IDRegistro), {
      aluno: IDAluno,
      hora: this.date,
      autorizadoPor: autorizadoPor,
      status: "foraDaEscola"
    });

    console.log("Exit registred with success.")
  }
}

const alteracao = new Catraca("LFplnjWNSN0amNh0Da79")

alteracao.ForaDaEscola("1238", "Deus")


//buttonEnter.addEventListener("click", () => entrada(text.value));
//buttonExit.addEventListener("click", () => saida(text.value));
