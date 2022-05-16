// using namespace std ;
// #define MAXN 101
// 用name[N][0]来存长度,name[N][1+]储存信息

//	 id    id_string
int pre[101][101][101][101] ;//课程
//       id    set   id   id_string
int name[101][101] ;
int credit[101] ; //学分
int score[101] ;  //成绩
int tried[101] ;  //上过的课
int sum_score = 0;       // GPA
int sum_credit_tried = 0;//尝试学分
int sum_credit_get = 0;  //已修学分
int gratuate_credit = 0 ;//还剩学分
int cur_pre_set[101][101] ;
int cur_pre[101];
int tmp ;
int cnt = 1 ;

int main() {
    int inn;
    int ikk;
    int jkk;
    scanf("%c", tmp);
    while(1) {
//        printf("%d %c\n", tmp, tmp);
        if ( tmp == 10 || tmp == 13){
            //if ( tmp == '\n' || tmp == '\r'){
            break ;
        }
//
        name[cnt][0] = 0 ;
        //记录课程名称string
        while (tmp != 124) {
            //while ( tmp != '|' ) {
           name[cnt][0] = name[cnt][0] + 1 ;
           name[cnt][ name[cnt][0] ] = tmp ;
           scanf("%c", tmp);
        }
//
//        // cout << name[cnt] << endl ;
//
        scanf("%d", credit[cnt]);
        //记录该门课学分
        // printf("credit = %d\n", credit[cnt]) ;

        scanf("%c", tmp);//跳过|
        scanf("%c", tmp);//[pre]*|[score]*

        while ( tmp != 124 ) {
            //while ( tmp != '|' ) {

            //		 id	id_string
            cur_pre_set[0][0] = 0 ;
            //  (,);(,,)|
            while ( tmp != 59 && tmp != 124 ) {
                //while ( tmp != ';' && tmp != '|') {

                cur_pre[0] = 0 ; //cur_pre[0]记录该课程名长度
                while ( tmp != 44 && tmp != 59 && tmp != 124 ) {
                    //while ( tmp != ',' && tmp != ';' && tmp != '|') {
                    cur_pre[0] = cur_pre[0] + 1 ;//cur_pre[1+]记录该课程名
                    cur_pre[cur_pre[0]] = tmp ;
                    scanf("%c", tmp);
                    // puts("doing ','") ;
                }
                cur_pre_set[0][0] = cur_pre_set[0][0] + 1 ;//记录课程个数
                cur_pre_set[ cur_pre_set[0][0] ][0] = cur_pre[0] ;//[][]记录课程名称长度
                inn = 1;
                while (inn<=cur_pre[0]) {
                    cur_pre_set[ cur_pre_set[0][0] ][inn] = cur_pre[inn] ;
                    inn=inn+1;
                }
                if ( tmp == 44 ) {
                    //if (tmp == ',')
                    scanf("%c", tmp);
                }
            }
            pre[cnt][0][0][0] = pre[cnt][0][0][0] + 1 ;//
            pre[cnt][ pre[cnt][0][0][0] ][0][0] = cur_pre_set[0][0] ;
            ikk=1;
            while (ikk <= cur_pre_set[0][0]) {
                pre[cnt][ pre[cnt][0][0][0] ][ikk][0] = cur_pre_set[ikk][0] ;
                jkk=1;
                while ( jkk <= cur_pre_set[ikk][0]) {
                    pre[cnt][ pre[cnt][0][0][0] ][ikk][jkk] = cur_pre_set[ikk][jkk] ;
                    jkk=jkk+1;
                }
                ikk= ikk + 1;
            }

            // pre[cnt].push_back(cur_pre_set) ;
            if (tmp == 59) {
                scanf("%c", tmp);
            }
            // puts("doing '|'") ;
        }
        scanf("%c", tmp);

        tried[cnt] = 1 ;//第cnt节课是不是上过了

        if (tmp == 65){
            //if (tmp == 'A'){
            score[cnt] = 4 ;
            scanf("%c", tmp);
        }
        else {
            if (tmp == 66) {
                //if (tmp == 'B'){
                score[cnt] = 3 ;
                scanf("%c", tmp);
            }
            else {
                if (tmp == 67) {
                    //if (tmp == 'C'){
                    score[cnt] = 2 ;
                    scanf("%c", tmp);
                }
                else {
                    if (tmp == 68) {
                        //if (tmp == 'D'){
                        score[cnt] = 1 ;
                        scanf("%c", tmp);
                    }
                    else{
                        if (tmp == 70) {
                            //if (tmp == 'F'){
                            score[cnt] = 0 ;
                            scanf("%c", tmp);
                        }
                        else {
                            tried[cnt] = 0 ;
                            score[cnt] = 0 ;
                        }
                    }
                }
            }
        }

        gratuate_credit = gratuate_credit + credit[cnt] ;
        if (tried[cnt] == 1) {
            sum_credit_tried = sum_credit_tried + credit[cnt] ;
            sum_score = sum_score + score[cnt] * credit[cnt];
            if (score[cnt] > 0) {
                sum_credit_get = sum_credit_get + credit[cnt] ;
            }
        }

        scanf("%c", tmp);
        cnt = cnt + 1 ;

    }

    cnt = cnt - 1 ;


//     for (int i = 1; i <= cnt; ++i ) {
//     	cout << name[i] << endl ;
//     	cout << credit[i] << endl ;
//     	cout << "pre:\n" ;
//     	for ( int j = 0; j < pre[i].size(); ++j ) {
//     		printf("\n\tset:\n") ;
//     		for ( int k = 0; k < pre[i][j].size(); ++k ) {
//     			cout << "\t\t" << pre[i][j][k] << endl ;
//     		}
//     	}
//     	cout << "end\n" ;
//     }


     if ( sum_credit_tried == 0 ) {
         printf("GPA: 0.0\n");
     }
     else {
         //printf("GPA: %.5f\n", 1.0 * sum_score / sum_credit_tried) ;
         float a;
         float b;
         a = sum_score;
         b = sum_credit_tried;
         float output_gpa = a/b;
         printf("GPA: %.1f\n", output_gpa) ;
     }
     printf("Hours Attempted: %d\n", sum_credit_tried) ;
     printf("Hours Completed: %d\n", sum_credit_get) ;
     printf("Credits Remaining: %d\n\n", gratuate_credit - sum_credit_get) ;

     printf("Possible Courses to Take Next\n") ;
     if (gratuate_credit == sum_credit_get ) {
         printf("  None - Congratulations!\n") ;
     }
     else {
         int i=1;
         int tag;
         int j;
         int k;
         int cur[101];
         int ii;
         int tag_cur;
         while (i <= cnt){
             if (score[i]==0) {
                 tag = 0 ;
                 if (pre[i][0][0][0] == 0) {
                     tag = 1;
                 }
                 j=1;
                 while (j <= pre[i][0][0][0]) {
                     // cout << "n = " << int(pre[i][j][0][0]) << endl ;
                     tag = 1;
                     k=1;
                     while (k <= pre[i][j][0][0]){
                         cur[0] = pre[i][j][k][0] ;
                         ii=1;
                         while (ii <= pre[i][j][k][0]){
                             cur[ii] = pre[i][j][k][ii] ;
                             ii=ii+1;
                         }

                         // cout << '\t' << &cur[1] << endl ;
                         int found = 0 ;
                         int l=1;
                         while (l<=cnt){
                             tag_cur = 1 ;
                             if (name[l][0] != cur[0] ) {
                                 tag_cur = 0 ;
                             }
                             else {
                                 ii=1;
                                 while (ii <= cur[0]) {
                                     if ( name[l][ii] != cur[ii] ) {
                                         tag_cur = 0 ;
                                         break ;
                                     }
                                     ii=ii+1;
                                 }
                             }
                             if (tag_cur) {
                                 // cout << &(name[l][1]) << endl ;
                                 // cout << &(cur[1]) << endl<<endl ;
                                 //
                                 found = 1 ;
                                 // printf("score %d %d\n", score[l], l);
                                 if (score[l] == 0) {
                                     tag = 0;
                                 }
                                 break ;
                             }
                             l=l+1;
                         }// l结束
                         if (found == 0) {
                             tag = 0;
                         }
                         k=k+1;
                     }// k 结束
                     if ( tag == 1 ) {
                         break;
                     }
                     j=j+1;
                 }//for 结束
                 if ( tag == 1 ) {
                     printf("  ") ;
                     int idx_tag = 1;
                     while ( idx_tag <= name[i][0] ){
                         printf("%c", name[i][idx_tag]) ;
                         idx_tag = idx_tag+1;
                     }
                     printf("\n") ;
                     // cout << name[i] << endl ;
                 }
             //     //getchar() ;
             }
             i=i+1;
         }//for 结束
     }

    return 0 ;
}