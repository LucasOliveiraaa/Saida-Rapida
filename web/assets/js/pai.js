import { initializeApp } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-app.js";
import { getFirestore, doc, collection, onSnapshot, query, where, orderBy, limit, getDocs, getDoc, setDoc } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-firestore.js";

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


const studentUi = document.querySelector("#student-list")

let alunos = []
let arrayaleatorio = []

let unsubscribeResponsavel //Vai na fé
let unsubscribeDependents = onSnapshot(collection(db, "NIL"), () => { }) //ta funcionando, não mexe

class Consulta {
  constructor(IDEscola) {
    this.escola = doc(db, "escolas", IDEscola)
  }

  getDependents(responsavel) {
    unsubscribeResponsavel = onSnapshot(doc(this.escola, "responsaveis", responsavel), (doc) => {
      console.log(doc.get("dependentes"))

      this.getDependentsNameConfig(doc.get("dependentes"))
    })
  }

  getDependentsNameConfig(arrDependents) {
    console.log("dependentsnameconfig")
    /*    qDependents = query(collection(this.escola, "alunos"), where("turma", 'in', arrDependentes));
    
        unsubscribeDependents = onSnapshot(qDependents, (querySnapshot) => {
          alunos.length = 0 //limpa o array
      
          querySnapshot.forEach((doc) => { //adiciona todos as pessoas que sao do nono ano no array
            alunos.push([doc.get("nome"), doc.id])
          });
          console.log(`Alunos salvos: ${alunos}`)
          this.GetStudents()
        })*/
    arrayaleatorio = arrDependents

    unsubscribeDependents()
    unsubscribeDependents = onSnapshot(collection(consul.escola, "alunos"), (col) => {
      this.getDependentsName()
    })
  }

  async getDependentsName() {
    alunos.lenght = 0
    console.log(arrayaleatorio)

    arrayaleatorio.forEach(async (student, index) => {
      console.log(index)

      const docRef = doc(this.escola, "alunos", student);
      const docSnap = await getDoc(docRef);

      console.log("nome e id:", docSnap.get("nome"), docSnap.id);
      alunos.push([docSnap.get("nome"), docSnap.id])

      console.log(`Alunos salvos:`, alunos, student)

      if (arrayaleatorio.length == index + 1) this.GetStudents()
    })
  }

  async GetStudents() {
    const date = new Date
    alunos = this.RemoverDuplicata(alunos)
    alunos.forEach(async (aluno, index) => { //passa por cada aluno
      aluno.length = 2 //limpa o status na escola
      console.log(aluno)
      const q = query(collection(this.escola, "historico"), where("aluno", "==", aluno[1]), orderBy("hora", "desc"), limit(1)) //filtra e pega o salvo mais recente
      const querySnapshot = await getDocs(q) //cria um array com todos os docs

      if (!querySnapshot.empty) {
        const doc = querySnapshot.docs[0] //pega o doc mais recente

        const docHora = doc.get("hora")
        const docDate = docHora.toDate()
        console.log(docDate)
        console.log(date)

        if (date.getYear() == docDate.getYear() & date.getMonth() == docDate.getMonth() & date.getDate() == docDate.getDate()) {
          const status = doc.get("status")

          aluno.push(status)
        }
        else console.log("O aluno não entrou na escola hoje.")
      }
      else aluno.push(undefined)

      if (alunos.length == index + 1) {
        const alunosComStatus = alunos.filter((aluno) => aluno[2] != undefined)
        console.log(alunosComStatus)
        this.atualizarAlunos(alunosComStatus)
      }
    })
  }

  atualizarAlunos(alunos) { //alunos[nome, id, status]
    alunos.sort()

    studentUi.innerHTML = ""
    for (const [name, id, type] of alunos) {
      const li = document.createElement("li")
      li.innerText = name
      li.classList.add("student", type)

      li.addEventListener("click", () => {
        if (type === "naSala") {
          this.alterarHistorico(id, "Deus")
        }
      })

      studentUi.appendChild(li)
    }
  }

  RemoverDuplicata(arrTurmas) {
    const turmas = new Set(arrTurmas) //Set é um array sem duplicatas
    return [...turmas] //esse [...] transforma um set(e outros) em array
  }

  dateDate(IDAluno) {
    const year = new Date().getFullYear()
    const month = new Date().getMonth()
    const day = new Date().getDate()
    const hour = new Date().getHours()
    const minutes = new Date().getMinutes()
    const seconds = new Date().getSeconds()
    console.log(`dia: ${day}, mês: ${month}, ano: ${year}, hora: ${hour}, minuto ${minutes}, segundo: ${seconds}`)
    return `${day}${month}${year}${hour}${minutes}${seconds}${IDAluno}`
  }

  async alterarHistorico(alunoID, autorizadoPor) {
    await setDoc(doc(this.escola, "historico", this.dateDate(alunoID)), {
      aluno: alunoID,
      hora: new Date(),
      autorizadoPor: autorizadoPor,
      status: "autorizado"
    });
    console.log("status atualizado")
  }
}
const consul = new Consulta("LFplnjWNSN0amNh0Da79")

consul.getDependents("kgYLW1fleRzHiNQhSc94")

onSnapshot(collection(consul.escola, "historico"), () => { //verifica se o historico foi alterado
  consul.GetStudents()
})