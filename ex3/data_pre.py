from scipy import misc
import numpy as np

class load_img:
    ## load image of one class from one of three folder(coarse, fine, real)
    ## the size of outputdata is (1178, 12292) containing 4 poses data and flatted normalized images data.
    def __init__(self, folderName, className, endIdx):
        self.folderName = folderName
        self.className = className
        self.endIdx = endIdx
        self.imglist = []
        # 64*64*3 = 12288
        self.img_fla = []
        for i in range(0, self.endIdx + 1):
            i1 = str(i)
            self.imgpath = self.folderName + self.className + i1 + ".png"

            self.img = misc.imread(self.imgpath)
            self.imglist.append(self.img)

            self.f_img = np.ndarray.flatten(self.img)
            self.img_fla.append(self.f_img)

        '''Normalize RGB channel to zero mean and unit variable'''
        self.X = self.img_fla
        self.X = np.double(self.X)
        self.X -= np.mean(self.X, axis=0)
        self.X /= np.std(self.X, axis=0)
        self.nor_img = self.X
        where_are_nan = np.isnan(self.nor_img)
        self.nor_img[where_are_nan] = 0

        '''Load the poses.txt data and convert them to a 2D list'''
        self.pos_path = folderName + "poses.txt"
        self.pos_dat = open(self.pos_path, 'r')
        self.a = self.pos_dat.read()
        self.pos_dat = self.a.split()
        ## the first 2 cols are # and "reali.png", which should be removed
        self.pos_dat = np.reshape(self.pos_dat, (endIdx + 1, 6))
        self.pos_dat = [[item for index, item in enumerate(items) if index != 0] for items in self.pos_dat]
        self.pos_dat = [[item for index, item in enumerate(items) if index != 0] for items in self.pos_dat]
        self.pos_list = []
        for i in range(endIdx + 1):
            a = list(map(float, self.pos_dat[i]))
            self.pos_list.append(a)

        self.data = []
        self.nor_img = self.nor_img.tolist()
        for i in range(endIdx + 1):
            self.bb = self.pos_list[i].copy()
            self.aa = self.nor_img[i].copy()
            self.bb.extend(self.aa)
            self.data.append(self.bb)
        # where_are_nan = np.isnan(self.data)
        # self.data[where_are_nan] = 0
    # def loa_img(self):
    #     self.dat = load_img.data
    #     return self.dat

    #will the variance be conflict when they own the same name
    def tra_img(self):
        self.spl_path = "/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/real/training_split.txt"
        self.spl_indice = open(self.spl_path, 'r')
        self.a = self.spl_indice.read()
        self.b = self.a.split(",")
        self.ind_list = list(map(int, self.b))
        # len = len(ind_list)
        self.tra_img = []
        for i in self.ind_list:
            self.tra = self.data[i]
            self.tra_img.append(self.tra)
        return self.tra_img

    def tes_img(self):
        self.spl_path = "/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/real/training_split.txt"
        self.spl_indice = open(self.spl_path, 'r')
        self.a = self.spl_indice.read()
        self.b = self.a.split(",")
        self.ind_list = list(map(int, self.b))
        self.array = np.arange(1178)
        self.sup = list(set(self.array) - set(self.ind_list))
        self.tes_img = []
        for j in self.sup:
            self.tes = self.data[j]
            self.tes_img.append(self.tes)
        return self.tes_img


    def tra_pos(self):
        self.spl_path = "/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/exercise3/dataset/real/training_split.txt"
        self.spl_indice = open(self.spl_path, 'r')
        self.a = self.spl_indice.read()
        self.b = self.a.split(",")
        self.ind_list = list(map(int, self.b))
        # len = len(ind_list)
        self.tra_pos = []
        for i in self.ind_list:
            self.tra = self.pos_list[i].copy()
            self.tra_pos.append(self.tra)
        return self.tra_pos





'''input the pos_list containing only  quaternion poses of S_db'''
'''output is a 2D list, each row contain a indice of a pose from'''
'''S_db and a indice of another similar pose to find puller'''
def similarity(S_db_pos, S_tra_pos):
    #In the data set of S_db, find the one which is most similar to the other one

    sim_ind = [[0 for x in range(S_db_pos)] for y in range(S_tra_pos)]
    for i in len(S_db_pos):
        a = 100
        sim_ind[i][0] = i
        for j in len(S_tra_pos):
            theta = np.arccos(abs(S_db_pos[i].dot(S_tra_pos[j].T)))
            if(theta < a):
                sim_ind[i][1] = j
    return sim_ind


'''need to create a 3D list to put the batches in??'''
def batch_generator(S_db, S_tra, n, sim_ind):

    len_db = len(S_db)
    len_tra = len(S_tra)
    gen_ind_tra = np.arrange(len_tra)
    gen_ind_tra = np.random.shuffle(gen_ind_tra)
    gen_ind_tra = gen_ind_tra[0:n]

    # sim_ind = similarity(S_db_pos, S_tra_pos)

    batch = []
    for i in gen_ind_tra:
        anchor = S_tra[i]
        ##first dimension is anchor from S_tra, second is puller from S_db
        batch.append(anchor)
        puller = S_db[(sim_ind[i][1])]
        batch.append(puller)
        ##there are 2 types of pushers: it can either be the same object but a different
        ## from puller pose or a randomly chosen different object from S_db
        '''hypothesis that the shape of S_db is (5*267)*12292'''
        gen_ind_db_cla = np.random.randint(5)
        gen_ind_db = np.random.shuffle(gen_ind_db_cla)
        if(S_db[gen_ind_db_cla][gen_ind_db] != puller):
            pusher = S_db[gen_ind_db_cla][gen_ind_db]
        batch.append(pusher)

## should I use dictionary?
'''the shape of coa_ape(example) should be 266*12292'''
coa_ape = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/coarse/ape/", "coarse", 266)
coa_ben = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/coarse/benchvise/", "coarse", 266)
coa_cam = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/coarse/cam/", "coarse", 266)
coa_cat = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/coarse/cat/", "coarse", 266)
coa_duc = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/coarse/duck/", "coarse", 266)

fin_ape = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/fine/ape/", "fine", 1010)
fin_ben = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/fine/benchvise/", "fine", 1010)
fin_cam = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/fine/cam/", "fine", 1010)
fin_cat = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/fine/cat/", "fine", 1010)
fin_duc = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/fine/duck/", "fine", 1010)

rea_ape = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/real/ape/", "real", 1177)
rea_ben = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/real/benchvise/", "real", 1177)
rea_cam = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/real/cam/", "real", 1177)
rea_cat = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/real/cat/", "real", 1177)
rea_duc = load_img("/C:/user/asus/Desktop/Tracking and detection for CV/exercise3/dataset/real/duck/", "real", 1177)

'''5*267*12292'''
S_db = []
S_db.extend(coa_ape.data)
S_db.extend(coa_ben.data)
S_db.extend(coa_cam.data)
S_db.extend(coa_cat.data)
S_db.extend(coa_duc.data)

S_db_pos = []
S_db_pos.extend(coa_ape.pos_list)
S_db_pos.extend(coa_ben.pos_list)
S_db_pos.extend(coa_cam.pos_list)
S_db_pos.extend(coa_cat.pos_list)
S_db_pos.extend(coa_duc.pos_list)


S_tra = []
tra_ape = rea_ape.tra_img()
tra_ben = rea_ape.tra_img()
tra_cam = rea_ape.tra_img()
tra_cat = rea_ape.tra_img()
tra_duc = rea_ape.tra_img()
S_tra.extend(fin_ape.data())
S_tra.extend(tra_ape)
S_tra.extend(fin_ben.data())
S_tra.extend(tra_ben)
S_tra.extend(fin_cam.data())
S_tra.extend(tra_cam)
S_tra.extend(fin_cat.data())
S_tra.extend(tra_cat)
S_tra.extend(fin_duc.data())
S_tra.extend(tra_duc)

S_tra_pos = []
tra_ape = rea_ape.tra_pos()
tra_ben = rea_ben.tra_pos()
tra_cam = rea_cam.tra_pos()
tra_cat = rea_cat.tra_pos()
tra_duc = rea_duc.tra_pos()
S_tra_pos.extend(fin_ape.pos_list)
S_tra_pos.extend(fin_ben.pos_list)
S_tra_pos.extend(fin_cam.pos_list)
S_tra_pos.extend(fin_cat.pos_list)
S_tra_pos.extend(fin_duc.pos_list)
S_tra_pos.extend(tra_ape)
S_tra_pos.extend(tra_ben)
S_tra_pos.extend(tra_cam)
S_tra_pos.extend(tra_cat)
S_tra_pos.extend(tra_duc)






# S_tes = []
# S_tes.extend(rea_ape.tes_img())
# S_tes.extend(rea_ben.tes_img())
# S_tes.extend(rea_cam.tes_img())
# S_tes.extend(rea_cat.tes_img())
# S_tes.extend(rea_duc.tes_img())

#
# print("the shape of S_tes is:")
# n = np.shape(S_tes)
# print(n)
#print(np.shape(tra_duc))
################################################################

'''Task 1 '''
sim_ind = similarity(S_db_pos, S_tra_pos)
batch_100 = batch_generator(S_db, S_tra, 100, sim_ind)

##TO DO: save the data_list to a txt file.
# with open("S_db.txt", "w") as output:
#     output.write(str(S_db))
# with open("S_tra.txt", "w") as output:
#     output.write(str(S_tra))
# with open("S_tes.txt", "w") as output:
#     output.write(str(S_tes))
with open("batch_100.txt", "w") as output:
    output.write(str(batch_100))